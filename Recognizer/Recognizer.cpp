#include "Recognizer.h"
#define PINVOKELIB_EXPORTS
#include "tchar.h"
#include <strsafe.h>
#include <objbase.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

using namespace std;

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif // _UNICODE

using namespace tesseract;

tesseract::TessBaseAPI  *reconAPI; 
Pix *reconImagePix = NULL;

char *reconStr[1024];	//识别出来的字符串数组
int reconCount = 0 ;		//需要转换的字符串count

bool _tSetEnvVar(const _TCHAR *envVarName, const _TCHAR *envVarValue)
{
	//get
	_TCHAR *getEnvStr = NULL;
	size_t len;
	_tdupenv_s(&getEnvStr, &len, envVarName);

	//if never set
	if (getEnvStr == NULL)
	{
		//set
		_TCHAR setStr[MAX_PATH];
		_tcscpy_s(setStr, envVarName);

#ifdef _UNICODE
		_tcscat_s(setStr, L"=");
#else
		_tcscat_s(setStr, "=");
#endif // _UNICODE
		_tcscat_s(setStr, envVarValue);

		if (_tputenv(setStr) != 0)
			return false;
	}	
	else
	{
		size_t cmpRes = _tcscmp(getEnvStr, envVarValue);
		if (_tcscmp(getEnvStr, envVarValue))			//if cmp not same
		{
			//set
			_TCHAR setStr[MAX_PATH];
			_tcscpy_s(setStr, envVarName);

#ifdef _UNICODE
			_tcscat_s(setStr, L"=");
#else
			_tcscat_s(setStr, "=");
#endif // _UNICODE
			_tcscat_s(setStr, envVarValue);

			if (_tputenv(setStr) != 0)
			{
				free(getEnvStr);
				getEnvStr = NULL;
				return false;
			}

		}
	}
	free(getEnvStr);
	getEnvStr = NULL;
	return true;
}

bool _tSetEnvVar2CurModDir(const _TCHAR *envVarName)
{
	_TCHAR currMod[MAX_PATH];
	GetModuleFileName(NULL, currMod, MAX_PATH);
	tstring envStr;
	tstring currExeDir;
#ifdef _UNICODE
	wstring delimiter = L"/\\";
#else
	string delimiter = "/\\";
#endif
	envStr = currMod;
	string::size_type pos = envStr.find_last_of(delimiter);
	currExeDir = envStr.substr(0, pos);

	return _tSetEnvVar(envVarName, currExeDir.c_str());

}

Pix* getSubRectPixel(Pix* pixel, int x, int y, int w, int h)
{
	Box* box = boxCreate(x, y, w, h);
	Pix* cropped = pixClipRectangle(pixel, box, NULL);
	boxDestroy(&box);
	//int res = pixWrite("cropped.png", cropped, IFF_PNG);

	PIX *pixt1, *pixt2;
	pixt1 = pixScale(cropped, 4, 4);
	//res = pixWrite("scaled.png", pixt1, IFF_PNG);

	pixt2 = pixUnsharpMasking(pixt1, 6, 0.5);
	//res = pixWrite("smoothed.png", pixt2, IFF_PNG);
	pixDestroy(&pixt1);

	return pixt2;
}

char* getReconText(Pix* imagePixel, int x, int y, int w, int h)
{
	if (imagePixel == NULL)
	{
		return NULL;
	}
	Pix* tempPix = NULL;
	tempPix = getSubRectPixel(imagePixel, x, y, w, h);

	reconAPI->SetImage(tempPix);
	char* resText = reconAPI->GetUTF8Text();

	char* reconText = NULL;
	reconText = (char*)malloc(strlen(resText)+1);
	strcpy_s(reconText, strlen(resText) + 1, resText);

	
	delete resText;
	resText = NULL;
	
	pixDestroy(&tempPix);
	tempPix = NULL;
	reconAPI->Clear();
	

	return reconText;
}

 bool __stdcall setImagePath(const char *imagePath)
{
	if (imagePath == NULL)
	{
		return false;
	}
	reconAPI = new tesseract::TessBaseAPI();
	char *configs = "digits";
	//reconAPI->SetVariable("tessedit_char_whitelist", "0123456789");
	/*reconAPI->SetVariable( "tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-,.:\D/@=μ" ); */

	reconAPI->Init(NULL, "eng", tesseract::OEM_DEFAULT);  
	reconImagePix = pixRead(imagePath);
	//reconAPI->SetImage(reconImage);
	return true;
}

 bool __stdcall setRecoRectGroup(RECTPARAM *rectGroup, int count)
{
	reconCount = count;
	for (int i = 0; i < reconCount; i ++ )
	{
		reconStr[i] = NULL;

		//reconAPI->SetRectangle(rectGroup[i].left, rectGroup[i].top, rectGroup[i].width, rectGroup[i].height);
		reconStr[i] = getReconText(reconImagePix, rectGroup[i].left, rectGroup[i].top, rectGroup[i].width, rectGroup[i].height);
	}

	return true;
}

 bool __stdcall getRecoStr_before(char ***resStr, int *count)
{
	*count = reconCount;
	size_t stSizeOfArray = sizeof(char*) * reconCount;

	*resStr = (char**)::CoTaskMemAlloc(stSizeOfArray);
	memset (*resStr, 0, stSizeOfArray);

	for (int i = 0; i < reconCount; i++)
	{
		(*resStr)[i] = (char*)::CoTaskMemAlloc(strlen(reconStr[i]) + 1);
		strcpy_s((*resStr)[i], strlen(reconStr[i]) + 1, reconStr[i]);
	}
	//reconAPI->Clear();
	return true;
}

 bool __stdcall getRecoStr(char *resStr[], int count)
{
	int result = 0;
	STRSAFE_LPSTR temp;
	size_t len;
	size_t alloc_size = 0;

	for ( int i = 0; i < count; i++ )
	{
		alloc_size = sizeof(char) * strlen(reconStr[i])/*RECONSTRLEN*/;
		//len = 0;
		//StringCchLengthA( resStr[i], STRSAFE_MAX_CCH, &len );
		//result += len;

		temp = (STRSAFE_LPSTR)CoTaskMemAlloc( alloc_size );
		StringCchCopyA( temp, alloc_size, (STRSAFE_LPCSTR)reconStr[i] );

		if (reconStr[i] != NULL)
		{
			delete reconStr[i];
			reconStr[i] = NULL;
		}

		// CoTaskMemFree must be used instead of delete to free memory.

		CoTaskMemFree( resStr[i] );
		resStr[i] = (char *) temp;
	}
	return true;
}

 bool __stdcall getSingleImageStr(char *resStr, int capacity)
{
	char *tempStr = reconAPI->GetUTF8Text();

	const size_t alloc_size = 1024;
	//STRSAFE_LPSTR result = (STRSAFE_LPSTR)CoTaskMemAlloc( alloc_size );
	//STRSAFE_LPCSTR teststr = resStr;
	//StringCchCopyA( result, alloc_size, tempStr );
	//CoTaskMemFree(resStr);
	strcpy_s(resStr, capacity, tempStr);

	delete tempStr;
	tempStr = NULL;

	reconAPI->Clear();

	return true;

}

RECOGNIZER_EXPORTS bool __stdcall setSingleImage(const char *imagePath)
{
	if (imagePath == NULL)
	{
		return false;
	}

	if (!_tSetEnvVar2CurModDir(L"TESSDATA_PREFIX"))
	{
		return false;
	}

	reconAPI = new tesseract::TessBaseAPI();

	reconAPI->Init(NULL, "eng", tesseract::OEM_DEFAULT);  
	reconImagePix = pixRead(imagePath);
	//reconAPI->SetImage(reconImagePix);

	return true;
}

RECOGNIZER_EXPORTS bool __stdcall setRecoRectangle(int left, int top, int width, int height)
{
	//reconAPI->SetRectangle(left, top, width, height);
	reconStr[0] = NULL;

	reconStr[0] = getReconText(reconImagePix, left, top, width, height);

	reconCount += 1;
	return true;
}

RECOGNIZER_EXPORTS char* __stdcall getImageStr()
{
	int result = 0;
	STRSAFE_LPSTR temp;
	size_t len;
	size_t alloc_size = 0;

	alloc_size = sizeof(char) * strlen(reconStr[0])/*RECONSTRLEN*/;
	//len = 0;
	//StringCchLengthA( resStr[0], STRSAFE_MAX_CCH, &len );
	//result += len;

	temp = (STRSAFE_LPSTR)CoTaskMemAlloc( alloc_size );
	StringCchCopyA( temp, alloc_size, (STRSAFE_LPCSTR)reconStr[0] );

	if (reconStr[0] != NULL)
	{
		delete reconStr[0];
		reconStr[0] = NULL;
	}

	reconAPI->Clear();

	return temp;
}

RECOGNIZER_EXPORTS bool __stdcall clearOCR()
{
	//reconAPI->Clear();
	reconAPI->End();
	pixDestroy(&reconImagePix);
	reconImagePix = NULL;

	delete reconAPI;
	reconAPI = NULL;

	for (int i = 0; i < reconCount; i ++ )
	{
		reconStr[i] = NULL;
	}

	return true;
}

