using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Drawing;

/*******************************
 * 
 * 负责读取默认位置的图像，
 * 调用OCR模块进行识别，
 * 并将识别结果放入剪贴板
 * 
*******************************/
namespace OCR
{
    class GetString
    {
        //NGA
        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("Recognizer.dll")]
        public static extern bool setSingleImage(String filePath);

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("Recognizer.dll")]
        public static extern bool setRecoRectangle(Int32 left, Int32 top, Int32 width, Int32 height);

        [DllImport("Recognizer.dll")]
        public static extern string getImageStr();

        [return: MarshalAs(UnmanagedType.I1)]
        [DllImport("Recognizer.dll")]
        public static extern bool clearOCR();

        //string filePath = @"1.png";
        public string myGetString(string path)
        {
            Bitmap myBitmap = new Bitmap(path);
            int height = myBitmap.Height;
            int width = myBitmap.Width;

            //NGA
            bool res = setSingleImage(path);
            res = setRecoRectangle(0, 0, width, height);

            string resStr = getImageStr();
            clearOCR();
            //MessageBox.Show(resStr);
            //string resStr = "";
            //getSingleImageStr(resStr);
            //Clipboard.SetDataObject(resStr.ToString());
            return resStr.ToString();
        }

    }
}
