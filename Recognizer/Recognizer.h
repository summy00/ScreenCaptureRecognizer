#ifndef GVOCR_H
#define GVOCR_H

#ifdef RECOGNIZER_EXPORTS
#define RECOGNIZER_EXPORTS extern "C" __declspec(dllexport)
#else
#define RECOGNIZER_EXPORTS extern "C" __declspec(dllimport)
#endif

#include "tesseract\baseapi.h"  
#include "tesseract\strngs.h" 
#include "leptonica\allheaders.h"
#include "tesseract\basedir.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

#define RECONSTRLEN 1024

typedef struct _RECTPARAM
{
	int left;
	int top;
	int width;
	int height;
}RECTPARAM;

 bool __stdcall setImagePath(const char *imagePath);

 RECOGNIZER_EXPORTS bool __stdcall setRecoRectangle(int left, int top, int width, int height);

 bool __stdcall setRecoRectGroup(RECTPARAM *rectGroup, int count);

 bool __stdcall getRecoStr_before(char ***resStr, int *count);
 bool __stdcall getRecoStr(char *resStr[], int count);

RECOGNIZER_EXPORTS bool __stdcall clearOCR();

//for NGA
RECOGNIZER_EXPORTS bool __stdcall setSingleImage(const char *imagePath);

bool __stdcall getSingleImageStr(char *resStr, int capacity);

RECOGNIZER_EXPORTS char* __stdcall getImageStr();







#endif