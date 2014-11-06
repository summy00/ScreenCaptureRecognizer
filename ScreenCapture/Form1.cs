using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
namespace OCR
{

    public partial class Form1 : Form
    {
        const int ballonStayTime = 300;
        bool is_OCR_in_using = true;
        GetString orcMgr;
        public Form1()
        {
            InitializeComponent();
        }

        [System.Runtime.InteropServices.DllImport("user32")]
        private static extern IntPtr SetClipboardViewer(IntPtr hwnd);

        [System.Runtime.InteropServices.DllImport("user32")]
        private static extern IntPtr ChangeClipboardChain(IntPtr hwnd, IntPtr hWndNext);

        [System.Runtime.InteropServices.DllImport("user32")]
        private static extern int SendMessage(IntPtr hwnd, int wMsg, IntPtr wParam, IntPtr lParam);
        const int WM_DRAWCLIPBOARD = 0x308;
        const int WM_CHANGECBCHAIN = 0x30D;

        IntPtr NextClipHwnd;
        private void Form1_Load(object sender, System.EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
            this.ShowInTaskbar = false;

            toolStripMenuItem1.CheckState = CheckState.Checked;
            notifyIcon1.BalloonTipText = "OCR已开启";
            notifyIcon1.ShowBalloonTip(ballonStayTime);

            //实例化ORC
            orcMgr = new GetString();

            //获得观察链中下一个窗口句柄
            NextClipHwnd = SetClipboardViewer(this.Handle);
        }

        protected override void WndProc(ref System.Windows.Forms.Message m)
        {
            switch (m.Msg)
            {
                case WM_DRAWCLIPBOARD:
                    //将WM_DRAWCLIPBOARD消息传递到下一个观察链中的窗口
                    SendMessage(NextClipHwnd, m.Msg, m.WParam, m.LParam);

                    if (is_OCR_in_using)
                        ClipboardToString();
                    break;
                default:
                    base.WndProc(ref m);
                    break;
            }
        }
        private void Form1_FormClosed(object sender, System.EventArgs e)
        {
            //从观察链中删除本观察窗口（第一个参数：将要删除的窗口的句柄；第二个参数：//观察链中下一个窗口的句柄 ）
            ChangeClipboardChain(this.Handle, NextClipHwnd);

            //将变动消息WM_CHANGECBCHAIN消息传递到下一个观察链中的窗口
            SendMessage(NextClipHwnd, WM_CHANGECBCHAIN, this.Handle, NextClipHwnd);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            ClipboardToString();
        }

        private void Form1_SizeChanged(object sender, EventArgs e)
        {
            if (this.WindowState == FormWindowState.Minimized)
            {
                this.Hide();   //隐藏窗体
                notifyIcon1.Visible = true; //使托盘图标可见
            }
        }

        private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //判断可有可无，因为目前的代码出现托盘的时候只会为最小化
            if (this.WindowState == FormWindowState.Minimized)
            {
                //还原
                this.Visible = true;
                this.WindowState = FormWindowState.Normal;
            }
            this.Activate();
            this.notifyIcon1.Visible = false;
            this.ShowInTaskbar = true;
        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            if (toolStripMenuItem1.Checked)
            {
                toolStripMenuItem1.CheckState = CheckState.Checked;
                is_OCR_in_using = true;
                notifyIcon1.BalloonTipText = "OCR已开启";
                notifyIcon1.ShowBalloonTip(ballonStayTime);

            }
            else
            {
                toolStripMenuItem1.CheckState = CheckState.Unchecked;
                is_OCR_in_using = false;
                notifyIcon1.BalloonTipText = "OCR已关闭";
                notifyIcon1.ShowBalloonTip(ballonStayTime);
            }
        }

        private void toolStripMenuItem2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void ClipboardToString()
        {
            IDataObject iData = Clipboard.GetDataObject();

            //检测文本
            if (iData.GetDataPresent(DataFormats.Text) | iData.GetDataPresent(DataFormats.OemText))
            {
                //this.richTextBox1.Text = (String)iData.GetData(DataFormats.Text);
            }

            //检测图像

            if (iData.GetDataPresent(DataFormats.Bitmap))
            {
                //this.Show();
                Image img = Clipboard.GetImage();
                pictureBox1.Image = img;

                string filePath = DateTime.Now.ToString("hmmss") + ".png";
                img.Save(filePath);

                string str = orcMgr.myGetString(filePath);
                Clipboard.SetDataObject(str);

                richTextBox1.Text = str;
                notifyIcon1.BalloonTipText = str;
                notifyIcon1.ShowBalloonTip(ballonStayTime);

                //需ORC底层在识别图像后并释放占用及内存后才能在上层进行文件删除操作
                //System.IO.File.Delete(filePath);
            }

            //检测自定义类型
            if (iData.GetDataPresent("myFormat"))
            {
                //MyObj myobj = (MyObj)iData.GetData("myFormat");
                //this.richTextBox1.Text = myobj.ObjName;
            }
        }

    }
}
