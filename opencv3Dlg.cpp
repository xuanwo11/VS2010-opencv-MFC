
// opencv3Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "opencv3.h"
#include "opencv3Dlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include "cv.h"
#include "highgui.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "resource.h"
#include <atlconv.h>
#include <iostream>
#include "atlimage.h"
using namespace std;  
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//定义全局变量
Mat frame,IMG;  
CvvImage cimg1,cimg2;  
VideoCapture cap(0);
//CvCapture* Capture;
//CvCapture* m_Video;
IplImage* TheImage;
IplImage img1,img2;
CRect rect;
CDC *pDC;
HDC hDC;
CWnd *pwnd;

Mat g_srcImage,g_dstImage,imginf;
////////////////////////////腐蚀膨胀,Canny,亮度对比度，
int g_nTrackbarNumber = 0;//0表示腐蚀，1表示膨胀
int g_nStructElementSize = 3;//内核矩阵尺寸
//void Process();
void on_TrackbarNumChange(int,void*);
void on_ElementSizeChange(int,void*);
///////////////////////////人脸检测
int FaceTest();

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Copencv3Dlg 对话框

Copencv3Dlg::Copencv3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Copencv3Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Copencv3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Copencv3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_EDGE, &Copencv3Dlg::OnVideoCanny)
    ON_BN_CLICKED(IDOK, &Copencv3Dlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &Copencv3Dlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDCANCEL2, &Copencv3Dlg::OnBnClickedCancel2)
    ON_BN_CLICKED(ID_EXIT, &Copencv3Dlg::OnCancel)
    ON_WM_TIMER(ID_FACE_DET, &Copencv3Dlg::OnFaceDet)
    //ON_BN_CLICKED(IDOK, &Copencv3Dlg::OnBnClickedOk)
    ON_WM_TIMER(ID_FILE_OPEN，&Copencv3Dlg::OnFileOpen)
    ON_WM_TIMER(ID_FILE_SAVE，&Copencv3Dlg::OnFileSave)
    ON_WM_TIMER(ID_INFLATE, &Copencv3Dlg::OnInflate)
    ON_WM_TIMER(IDOK2, &Copencv3Dlg::Process)
    ON_WM_TIMER(ID_PCANNY, &Copencv3Dlg::OnPicCanny)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// Copencv3Dlg 消息处理程序
int Copencv3Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (Copencv3Dlg::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
    
	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	
	return 0;
}

BOOL Copencv3Dlg::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !Copencv3Dlg::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

#ifdef _DEBUG
void Copencv3Dlg::AssertValid() const
{
	CDialogEx::AssertValid();
}

void Copencv3Dlg::Dump(CDumpContext& dc) const
{
	CDialogEx::Dump(dc);
}

#endif //_DEBUG

BOOL Copencv3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



void Copencv3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Copencv3Dlg::OnPaint()
{

    if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();//背景图片设置
        CRect rc;  
        GetWindowRect(&rc);  
        ATL::CImage mImage;  
        if(mImage.Load(_T("bg.jpg")) == S_OK)  
        {  
            //这里让图片绘制区域和窗口大小一致  
            mImage.Draw(GetDC()->GetSafeHdc(), CRect(0,0,rc.Width(),rc.Height()));  
        } 
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Copencv3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Copencv3Dlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	//if (wParam == ID_FILE_SAVE_AS)
	//	OnFileSaveAs();
	if (wParam == ID_FILE_OPEN)
		OnFileOpen();
    else if (wParam == ID_FILE_SAVE)
        OnFileSave();
    else if (wParam == IDCANCEL)
        OnBnClickedCancel();
    else if (wParam == IDCANCEL2)
        OnBnClickedCancel2();
    else if (wParam == IDOK)
        OnBnClickedOk();
    else if (wParam == ID_INFLATE)
        OnInflate();
    else if (wParam == IDOK2)
        Process();
	else if (wParam == ID_EDGE)
        OnVideoCanny();
    else if (wParam == ID_FACE_DET)
        OnFaceDet();
    else if (wParam == ID_EXIT)
        OnCancel();
    else if (wParam == ID_PCANNY)
        OnPicCanny();
	return TRUE;

	//return CFrameWnd::OnCommand(wParam, lParam);
}

void Copencv3Dlg::OnCancel()
{
   CDialogEx::OnCancel();
}

void Copencv3Dlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
	CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	HDC hdc= pDC->GetSafeHdc();                      // 获取设备上下文句柄
    CRect rect;
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //获取box1客户区
    CvvImage cimg;
    
    // 打开图片文件
    CString m_strPicPath;
    //IplImage* TheImage;
    CFileDialog dlg(true,_T("*.bmp"),NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
                    _T("image file(*.bmp;*.jpg)|*.bmp;*.jpg|All Files(*.*)|*.*|"),NULL);//弹出选择图片对话框
    dlg.m_ofn.lpstrTitle=_T("open Image");
    if(dlg.DoModal()!=IDOK)
       return ;
     m_strPicPath = dlg.GetPathName();
     m_strPicPath.Replace(_T("//"),_T("////"));
     TheImage=cvLoadImage((CT2CA)m_strPicPath,1);
     if(TheImage!=NULL)
     {
         //ShowImage(TheImage, IDC_STATIC_PIC);
         CDC* pDC=GetDlgItem(IDC_STATIC)->GetDC();
         HDC hDC=pDC->GetSafeHdc();
         CRect rect;
         GetDlgItem(IDC_STATIC)->GetClientRect(&rect);
         SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
         CvvImage cimg;
         cimg.CopyOf(TheImage);
         cimg.DrawToHDC(hDC,&rect);
         ReleaseDC(pDC);
     }
     //cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
     //cvShowImage("Example1", img);
     //cvWaitKey(0);
     //cvReleaseImage(&img);
     //cvDestroyWindow("Example1");    
/*    CRect rect;
// 矩形类
   GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //获取box1客户区
   CvvImage cimg;
   IplImage *src; // 定义IplImage指针变量src     
    src = cvLoadImage("D:\\1.jpg",-1); // 将src指向当前工程文件目录下的图像me.bmp    
   cimg.CopyOf(src,src->nChannels);

   cimg.DrawToHDC(hdc,&rect);
//输出图像
   ReleaseDC( pDC );
   cimg.Destroy();
//销毁
    // TODO: 在此添加控件通知处理程序代码
    CDialogEx::OnOK();
  */ 
}

///////////////////////////////////////////////////////////////////////视频
void Copencv3Dlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
	 //IplImage *src; // 定义IplImage指针变量src     
  //  src = cvLoadImage("D:\\me.bmp",-1); // 将src指向当前工程文件目录下的图像me.bmp    
  //  cvNamedWindow("me",0);//定义一个窗口名为lena的显示窗口    
  //  cvShowImage("me",src);//在lena窗口中，显示src指针所指向的图像    
  //  cvWaitKey(0);//无限等待，即图像总显示    
  //  cvDestroyWindow("me");//销毁窗口lena    
  //  cvReleaseImage(&src);//释放IplImage指针src   
    CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针       
    HDC hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄      
    CRect rect;// 矩形类     
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect);   
  
    if (cap.isOpened())  
    {  
        cap >> frame;  
        img1 = IplImage(frame);  
        cimg1.CopyOf(&img1);  
        cimg1.DrawToHDC(hdc, &rect);  
        SetTimer(1, 25, NULL);//第一个1为计时器名称，第二个为时间间隔，单位毫秒  
    }  
/*	CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	 HDC hdc= pDC->GetSafeHdc();                      // 获取设备上下文句柄
	 CRect rect;
// 矩形类
   GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //获取box1客户区


	 //CvCapture *capture = cvCreateFileCapture ("D:\\ccbr1.avi");  //读取视频
   CvCapture* capture=cvCreateCameraCapture(0);
  
    if(capture==NULL) {
     printf("NO capture");    //读取不成功，则标识
     //return 1;
   };    
    double fps=cvGetCaptureProperty(capture, CV_CAP_PROP_FPS );   //读取视频的帧率
 int vfps = 1000 / fps;                                        //计算每帧播放的时间
    printf("%5.1f\t%5d\n",fps,vfps);                             
 double frames=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);//读取视频中有多少帧
 printf("frames is %f\n",frames);
 //cvNamedWindow("example",CV_WINDOW_AUTOSIZE);                  //定义窗口
 IplImage *frame;

   CvvImage cimg;

 while(1){ 
    frame = cvQueryFrame( capture );                          //抓取帧
    cimg.CopyOf(frame,frame->nChannels);
    cimg.DrawToHDC(hdc,&rect);
  float ratio = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO);     //读取该帧在视频中的相对位置
  printf("%f\n",ratio);
  if(!frame)break;
  //cvShowImage("IDC_STATIC",frame);                          //显示
  
  char c = cvWaitKey(vfps);
  if(c == 27 )break;
    
  // 设置计时器,每10ms触发一次事件  
    SetTimer(1, 25, NULL); //定时器，定时时间和帧率一致
 }
    ReleaseDC(pDC);
    cvReleaseCapture(&capture);
    cvDestroyWindow("example");
    // TODO: 在此添加控件通知处理程序代码
   // CDialogEx::OnCancel();
   */
}

void Copencv3Dlg::OnBnClickedCancel2()
{
    // TODO: 在此添加控件通知处理程序代码
    // 暂停  
    KillTimer(1);//不使用计时器 
}


void Copencv3Dlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针       
    HDC hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄      
    CRect rect;// 矩形类     
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect);   
    
    //CDC *pDC2 = GetDlgItem(IDC_NEW_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针       
    //HDC hdc2 = pDC2->GetSafeHdc();                      // 获取设备上下文句柄      
    //CRect rect2;// 矩形类     
    //GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect2);

    if (cap.isOpened())  
    {  
        cap >> frame;  
        img1 = IplImage(frame);
        //img2 = IplImage(frame);
        cimg1.CopyOf(&img1);  
        cimg1.DrawToHDC(hdc, &rect);
        //cimg2.CopyOf(&img2);  
        //cimg2.DrawToHDC(hdc2, &rect2);
        SetTimer(1, 25, NULL);  
    }  
    CDialogEx::OnTimer(nIDEvent);
}

void Copencv3Dlg::OnFaceDet()
{
    //VideoCapture cap(0);    //打开默认摄像头 
    //namedWindow("人脸识别",WINDOW_AUTOSIZE);
    CDC *pDC2 = GetDlgItem(IDC_NEW_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针       
    HDC hdc2 = pDC2->GetSafeHdc();                      // 获取设备上下文句柄      
    CRect rect2;// 矩形类     
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect2);
      
    Mat gray;  
  
    CascadeClassifier cascade;  
    bool stop = false;  
    //训练好的文件名称，放置在可执行文件同目录下  
    cascade.load("haarcascade_frontalface_default.xml");  
  
    Ptr<FaceRecognizer> modelPCA = createEigenFaceRecognizer();  
    modelPCA->load("MyFacePCAModel.xml");  
      
    while(1)//if (cap.isOpened())  
    { 
        cap >> frame;  
        //建立用于存放人脸的向量容器  
        vector<Rect> faces(0);  
  
        cvtColor(frame, gray, CV_BGR2GRAY);  
        //改变图像大小，使用双线性差值  
        //resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);  
        //变换后的图像进行直方图均值化处理  
        equalizeHist(gray, gray);  
  
        cascade.detectMultiScale(gray, faces,  
            1.1, 2, 0  
            //|CV_HAAR_FIND_BIGGEST_OBJECT  
            //|CV_HAAR_DO_ROUGH_SEARCH  
            | CV_HAAR_SCALE_IMAGE,  
            Size(30, 30));//30  
  
        Mat face;  
        Point text_lb;  
  
        for (size_t i = 0; i < faces.size(); i++)  
        {  
            if (faces[i].height > 0 && faces[i].width > 0)  
            {  
                face = gray(faces[i]);  
                text_lb = Point(faces[i].x, faces[i].y);  
  
                rectangle(frame, faces[i], Scalar(255, 0, 0), 1, 8, 0); //1 
            }  
        }  
  
        Mat face_test;  
  
        int predictPCA = 0;  
        if (face.rows >= 120)  
        {  
            resize(face, face_test, Size(92, 112));  
              
        }  
        //Mat face_test_gray;  
        //cvtColor(face_test, face_test_gray, CV_BGR2GRAY);  
            //在这里对人脸进行判别 
          /*  int predictedLabel = -1;
            double predicted_confidence = 5.0;  
            modelPCA->predict(face_test,predictedLabel,predicted_confidence);  
            if(predictedLabel == 0) 
            {
                string name = "HuFuping";  
                putText(frame, name, text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));  
            }
            else 
            {
                string name = "No";  
                putText(frame, name, text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
            }
          */  
        ///////////////////////////////////////
        int predictedLabel = -1;
        double predicted_confidence = 4.0;  
        //modelPCA->predict(face_test,predictedLabel,predicted_confidence);
        if (!face_test.empty())  
        {  
            //测试图像应该是灰度图 
            modelPCA->predict(face_test,predictedLabel,predicted_confidence);
            //predictPCA = modelPCA->predict(face_test);  
        }  
        cout << predictPCA << endl; 
        if (predicted_confidence <= 6000)//predictPCA == 40,predictedLabel == 0,predicted_confidence >= 80  
        {  
            //ostringstream os;
            //os << predicted_confidence;
            //os.str();
            string name = "HuFuping";  
            putText(frame, name, text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));  
        } 
        else
        {
            string name = "No";  
            putText(frame, name, text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
        }
  /////////////////////////////////////////
        //imshow("人脸识别", frame);
        img2 = IplImage(frame);
        cimg2.CopyOf(&img2);  
        cimg2.DrawToHDC(hdc2, &rect2);
        //SetTimer(3, 1000, NULL);
        if (waitKey(30) >= 0)  
            break;//stop = true;  
    }  
    //return 0;
}

void Copencv3Dlg::OnVideoCanny()
{
    // TODO: 在此添加消息处理程序代码
    CDC *pDC2 = GetDlgItem(IDC_NEW_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针       
    HDC hdc2 = pDC2->GetSafeHdc();                      // 获取设备上下文句柄      
    CRect rect2;// 矩形类     
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect2);   
    VideoCapture capture(0);
    Mat edges;
    while(1)//if (cap.isOpened())  
    {  
        capture >> frame;  
        cvtColor(frame,edges,CV_BGR2GRAY);
        blur(edges,edges,Size(5,5));
        Canny(edges,edges,3,20,3);
        img2 = IplImage(edges);  
        cimg2.CopyOf(&img2);  
        cimg2.DrawToHDC(hdc2, &rect2);
        //if(waitKey(30) >= 0) break;
        SetTimer(2, 25, NULL);//第一个1为计时器名称，第二个为时间间隔，单位毫秒  
    } 
  
}

//////////////////////////////////////////////////////////////////视频
//////////////////////////////////////////////////////////////////图片
//打开文件选取
void Copencv3Dlg::OnFileOpen()
{
	 // TODO: 在此添加控件通知处理程序代码
	CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	HDC hdc= pDC->GetSafeHdc();                      // 获取设备上下文句柄
    CRect rect;
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //获取box1客户区
    CvvImage cimg;
    
    // 打开图片文件
    CString m_strPicPath;
    //IplImage* TheImage;
    CFileDialog dlg(true,_T("*.bmp"),NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
                    _T("image file(*.bmp;*.jpg)|*.bmp;*.jpg|All Files(*.*)|*.*|"),NULL);//弹出选择图片对话框
    dlg.m_ofn.lpstrTitle=_T("open Image");
    if(dlg.DoModal()!=IDOK)
       return ;
     m_strPicPath = dlg.GetPathName();
     m_strPicPath.Replace(_T("//"),_T("////"));
     TheImage=cvLoadImage((CT2CA)m_strPicPath,1);
     if(TheImage!=NULL)
     {
         //ShowImage(TheImage, IDC_STATIC_PIC);
         CDC* pDC=GetDlgItem(IDC_STATIC)->GetDC();
         HDC hDC=pDC->GetSafeHdc();
         CRect rect;
         GetDlgItem(IDC_STATIC)->GetClientRect(&rect);
         SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
         CvvImage cimg;
         cimg.CopyOf(TheImage);
         cimg.DrawToHDC(hDC,&rect);
         ReleaseDC(pDC);
     }
     
}
/////////////////////////////////保存图片功能
void Copencv3Dlg::OnFileSave()
{
    // TODO: 在此添加命令处理程序代码
	CFileDialog dlg(
		FALSE,
		_T("*.bmp"),
		_T("图像"),
		OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT,
		_T("保存为(*.jpg)|*.jpg|保存为(*.bmp)|*.bmp|所有文件(*.*)|*.*||"),
		NULL
		);
	CString mPath;
	if(dlg.DoModal()==IDOK)
	{
		mPath=dlg.GetPathName();
		USES_CONVERSION;
        String pathname = T2A(mPath.GetBuffer(0));
		//std::string pathname(T2A(mPath));
		imwrite(pathname,IMG);
	}
}

//腐蚀或膨胀处理///////////////////////////////////////////////////////////////
void Copencv3Dlg::OnInflate()//腐蚀/膨胀
{
    // TODO: 在此添加命令处理程序代码
    system("color5E");//改变字体颜色
    CDC* pDC=GetDlgItem(IDC_NEW_STATIC)->GetDC();
    HDC hDC=pDC->GetSafeHdc();
    CRect rect;
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect);
    SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
    CvvImage cimg;

    g_srcImage = cv::Mat(TheImage);
    if (g_srcImage.empty())
	{
		MessageBox(_T("未读入图像"),_T("警告"),MB_ICONWARNING);
		return;
	}
    namedWindow("参数调整",WINDOW_AUTOSIZE);
    Mat element = getStructuringElement(MORPH_RECT,Size(2*g_nStructElementSize+1,2*g_nStructElementSize+1),Point(g_nStructElementSize,g_nStructElementSize));
    erode(g_srcImage,g_dstImage,element);//初次腐蚀
    imshow("参数调整",g_dstImage);
    //创建滑动条
    createTrackbar("腐蚀/膨胀","参数调整",&g_nTrackbarNumber,1,on_TrackbarNumChange);
    createTrackbar("内核尺寸","参数调整",&g_nStructElementSize,21,on_ElementSizeChange);
    on_TrackbarNumChange(g_nTrackbarNumber,0);
    on_ElementSizeChange(g_nStructElementSize,0);

    TheImage = &IplImage(g_dstImage);
    cimg.CopyOf(TheImage);
    cimg.DrawToHDC(hDC,&rect);
    ReleaseDC(pDC);
}

void Copencv3Dlg::OnPicCanny()//Canny边缘检测
{
    CDC* pDC=GetDlgItem(IDC_NEW_STATIC)->GetDC();
    HDC hDC=pDC->GetSafeHdc();
    CRect rect;
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect);
    SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
    CvvImage cimg;

    Mat grayImage;
    g_srcImage = cv::Mat(TheImage);
    if (g_srcImage.empty())
	{
		MessageBox(_T("未读入图像"),_T("警告"),MB_ICONWARNING);
		return;
	}
    cvtColor(g_srcImage,grayImage,CV_BGR2GRAY);
    blur(grayImage,g_dstImage,Size(5,5));
    Canny(g_dstImage,g_dstImage,3,20,3);
    IMG = g_dstImage;//保存存储
    IplImage* CannyImg;
    CannyImg = &IplImage(g_dstImage);
    cimg.CopyOf(CannyImg);
    cimg.DrawToHDC(hDC,&rect);
    ReleaseDC(pDC);
}

void Copencv3Dlg::Process()
{
    // TODO: 在此添加命令处理程序代码
    CDC* pDC=GetDlgItem(IDC_NEW_STATIC)->GetDC();
    HDC hDC=pDC->GetSafeHdc();
    CRect rect;
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect);
    SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
    CvvImage cimg;   
    //imshow("参数调整",g_dstImage);
    IMG = imginf;//保存存储
    TheImage = &IplImage(imginf);
    cimg.CopyOf(TheImage);
    cimg.DrawToHDC(hDC,&rect);
    ReleaseDC(pDC);
}

void  on_TrackbarNumChange(int,void*)
{
    //&Copencv3Dlg::Process;
    Mat element = getStructuringElement(MORPH_RECT,Size(2*g_nStructElementSize+1,2*g_nStructElementSize+1),Point(g_nStructElementSize,g_nStructElementSize));
    if(g_nTrackbarNumber == 0){
        erode(g_srcImage,g_dstImage,element); 
    }
    else{
        dilate(g_srcImage,g_dstImage,element);
    }
    imshow("参数调整",g_dstImage);
    imginf = g_dstImage;
}
void  on_ElementSizeChange(int,void*)
{
    //&Copencv3Dlg::Process;
    Mat element = getStructuringElement(MORPH_RECT,Size(2*g_nStructElementSize+1,2*g_nStructElementSize+1),Point(g_nStructElementSize,g_nStructElementSize));
    if(g_nTrackbarNumber == 0){
        erode(g_srcImage,g_dstImage,element); 
    }
    else{
        dilate(g_srcImage,g_dstImage,element);
    }
    imshow("参数调整",g_dstImage);
    imginf = g_dstImage;
}
////////////////////////////////////////////////////////////////////