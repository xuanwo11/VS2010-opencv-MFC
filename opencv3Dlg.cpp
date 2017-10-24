
// opencv3Dlg.cpp : ʵ���ļ�
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

//����ȫ�ֱ���
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
////////////////////////////��ʴ����,Canny,���ȶԱȶȣ�
int g_nTrackbarNumber = 0;//0��ʾ��ʴ��1��ʾ����
int g_nStructElementSize = 3;//�ں˾���ߴ�
//void Process();
void on_TrackbarNumChange(int,void*);
void on_ElementSizeChange(int,void*);
///////////////////////////�������
int FaceTest();

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// Copencv3Dlg �Ի���

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
    ON_WM_TIMER(ID_FILE_OPEN��&Copencv3Dlg::OnFileOpen)
    ON_WM_TIMER(ID_FILE_SAVE��&Copencv3Dlg::OnFileSave)
    ON_WM_TIMER(ID_INFLATE, &Copencv3Dlg::OnInflate)
    ON_WM_TIMER(IDOK2, &Copencv3Dlg::Process)
    ON_WM_TIMER(ID_PCANNY, &Copencv3Dlg::OnPicCanny)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// Copencv3Dlg ��Ϣ�������
int Copencv3Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (Copencv3Dlg::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
    
	// TODO: �������Ҫ��ͣ������������ɾ��������
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	
	return 0;
}

BOOL Copencv3Dlg::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !Copencv3Dlg::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

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

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Copencv3Dlg::OnPaint()
{

    if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();//����ͼƬ����
        CRect rc;  
        GetWindowRect(&rc);  
        ATL::CImage mImage;  
        if(mImage.Load(_T("bg.jpg")) == S_OK)  
        {  
            //������ͼƬ��������ʹ��ڴ�Сһ��  
            mImage.Draw(GetDC()->GetSafeHdc(), CRect(0,0,rc.Width(),rc.Height()));  
        } 
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Copencv3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Copencv3Dlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
	CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��
	HDC hdc= pDC->GetSafeHdc();                      // ��ȡ�豸�����ľ��
    CRect rect;
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //��ȡbox1�ͻ���
    CvvImage cimg;
    
    // ��ͼƬ�ļ�
    CString m_strPicPath;
    //IplImage* TheImage;
    CFileDialog dlg(true,_T("*.bmp"),NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
                    _T("image file(*.bmp;*.jpg)|*.bmp;*.jpg|All Files(*.*)|*.*|"),NULL);//����ѡ��ͼƬ�Ի���
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
// ������
   GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //��ȡbox1�ͻ���
   CvvImage cimg;
   IplImage *src; // ����IplImageָ�����src     
    src = cvLoadImage("D:\\1.jpg",-1); // ��srcָ��ǰ�����ļ�Ŀ¼�µ�ͼ��me.bmp    
   cimg.CopyOf(src,src->nChannels);

   cimg.DrawToHDC(hdc,&rect);
//���ͼ��
   ReleaseDC( pDC );
   cimg.Destroy();
//����
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CDialogEx::OnOK();
  */ 
}

///////////////////////////////////////////////////////////////////////��Ƶ
void Copencv3Dlg::OnBnClickedCancel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
	 //IplImage *src; // ����IplImageָ�����src     
  //  src = cvLoadImage("D:\\me.bmp",-1); // ��srcָ��ǰ�����ļ�Ŀ¼�µ�ͼ��me.bmp    
  //  cvNamedWindow("me",0);//����һ��������Ϊlena����ʾ����    
  //  cvShowImage("me",src);//��lena�����У���ʾsrcָ����ָ���ͼ��    
  //  cvWaitKey(0);//���޵ȴ�����ͼ������ʾ    
  //  cvDestroyWindow("me");//���ٴ���lena    
  //  cvReleaseImage(&src);//�ͷ�IplImageָ��src   
    CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��       
    HDC hdc = pDC->GetSafeHdc();                      // ��ȡ�豸�����ľ��      
    CRect rect;// ������     
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect);   
  
    if (cap.isOpened())  
    {  
        cap >> frame;  
        img1 = IplImage(frame);  
        cimg1.CopyOf(&img1);  
        cimg1.DrawToHDC(hdc, &rect);  
        SetTimer(1, 25, NULL);//��һ��1Ϊ��ʱ�����ƣ��ڶ���Ϊʱ��������λ����  
    }  
/*	CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��
	 HDC hdc= pDC->GetSafeHdc();                      // ��ȡ�豸�����ľ��
	 CRect rect;
// ������
   GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //��ȡbox1�ͻ���


	 //CvCapture *capture = cvCreateFileCapture ("D:\\ccbr1.avi");  //��ȡ��Ƶ
   CvCapture* capture=cvCreateCameraCapture(0);
  
    if(capture==NULL) {
     printf("NO capture");    //��ȡ���ɹ������ʶ
     //return 1;
   };    
    double fps=cvGetCaptureProperty(capture, CV_CAP_PROP_FPS );   //��ȡ��Ƶ��֡��
 int vfps = 1000 / fps;                                        //����ÿ֡���ŵ�ʱ��
    printf("%5.1f\t%5d\n",fps,vfps);                             
 double frames=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);//��ȡ��Ƶ���ж���֡
 printf("frames is %f\n",frames);
 //cvNamedWindow("example",CV_WINDOW_AUTOSIZE);                  //���崰��
 IplImage *frame;

   CvvImage cimg;

 while(1){ 
    frame = cvQueryFrame( capture );                          //ץȡ֡
    cimg.CopyOf(frame,frame->nChannels);
    cimg.DrawToHDC(hdc,&rect);
  float ratio = cvGetCaptureProperty(capture, CV_CAP_PROP_POS_AVI_RATIO);     //��ȡ��֡����Ƶ�е����λ��
  printf("%f\n",ratio);
  if(!frame)break;
  //cvShowImage("IDC_STATIC",frame);                          //��ʾ
  
  char c = cvWaitKey(vfps);
  if(c == 27 )break;
    
  // ���ü�ʱ��,ÿ10ms����һ���¼�  
    SetTimer(1, 25, NULL); //��ʱ������ʱʱ���֡��һ��
 }
    ReleaseDC(pDC);
    cvReleaseCapture(&capture);
    cvDestroyWindow("example");
    // TODO: �ڴ���ӿؼ�֪ͨ����������
   // CDialogEx::OnCancel();
   */
}

void Copencv3Dlg::OnBnClickedCancel2()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    // ��ͣ  
    KillTimer(1);//��ʹ�ü�ʱ�� 
}


void Copencv3Dlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��       
    HDC hdc = pDC->GetSafeHdc();                      // ��ȡ�豸�����ľ��      
    CRect rect;// ������     
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect);   
    
    //CDC *pDC2 = GetDlgItem(IDC_NEW_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��       
    //HDC hdc2 = pDC2->GetSafeHdc();                      // ��ȡ�豸�����ľ��      
    //CRect rect2;// ������     
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
    //VideoCapture cap(0);    //��Ĭ������ͷ 
    //namedWindow("����ʶ��",WINDOW_AUTOSIZE);
    CDC *pDC2 = GetDlgItem(IDC_NEW_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��       
    HDC hdc2 = pDC2->GetSafeHdc();                      // ��ȡ�豸�����ľ��      
    CRect rect2;// ������     
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect2);
      
    Mat gray;  
  
    CascadeClassifier cascade;  
    bool stop = false;  
    //ѵ���õ��ļ����ƣ������ڿ�ִ���ļ�ͬĿ¼��  
    cascade.load("haarcascade_frontalface_default.xml");  
  
    Ptr<FaceRecognizer> modelPCA = createEigenFaceRecognizer();  
    modelPCA->load("MyFacePCAModel.xml");  
      
    while(1)//if (cap.isOpened())  
    { 
        cap >> frame;  
        //�������ڴ����������������  
        vector<Rect> faces(0);  
  
        cvtColor(frame, gray, CV_BGR2GRAY);  
        //�ı�ͼ���С��ʹ��˫���Բ�ֵ  
        //resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);  
        //�任���ͼ�����ֱ��ͼ��ֵ������  
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
            //����������������б� 
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
            //����ͼ��Ӧ���ǻҶ�ͼ 
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
        //imshow("����ʶ��", frame);
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
    // TODO: �ڴ������Ϣ����������
    CDC *pDC2 = GetDlgItem(IDC_NEW_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��       
    HDC hdc2 = pDC2->GetSafeHdc();                      // ��ȡ�豸�����ľ��      
    CRect rect2;// ������     
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
        SetTimer(2, 25, NULL);//��һ��1Ϊ��ʱ�����ƣ��ڶ���Ϊʱ��������λ����  
    } 
  
}

//////////////////////////////////////////////////////////////////��Ƶ
//////////////////////////////////////////////////////////////////ͼƬ
//���ļ�ѡȡ
void Copencv3Dlg::OnFileOpen()
{
	 // TODO: �ڴ���ӿؼ�֪ͨ����������
	CDC *pDC = GetDlgItem(IDC_STATIC)->GetDC();//����ID��ô���ָ���ٻ�ȡ��ô��ڹ�����������ָ��
	HDC hdc= pDC->GetSafeHdc();                      // ��ȡ�豸�����ľ��
    CRect rect;
    GetDlgItem(IDC_STATIC)->GetClientRect(&rect); //��ȡbox1�ͻ���
    CvvImage cimg;
    
    // ��ͼƬ�ļ�
    CString m_strPicPath;
    //IplImage* TheImage;
    CFileDialog dlg(true,_T("*.bmp"),NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
                    _T("image file(*.bmp;*.jpg)|*.bmp;*.jpg|All Files(*.*)|*.*|"),NULL);//����ѡ��ͼƬ�Ի���
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
/////////////////////////////////����ͼƬ����
void Copencv3Dlg::OnFileSave()
{
    // TODO: �ڴ���������������
	CFileDialog dlg(
		FALSE,
		_T("*.bmp"),
		_T("ͼ��"),
		OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT,
		_T("����Ϊ(*.jpg)|*.jpg|����Ϊ(*.bmp)|*.bmp|�����ļ�(*.*)|*.*||"),
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

//��ʴ�����ʹ���///////////////////////////////////////////////////////////////
void Copencv3Dlg::OnInflate()//��ʴ/����
{
    // TODO: �ڴ���������������
    system("color5E");//�ı�������ɫ
    CDC* pDC=GetDlgItem(IDC_NEW_STATIC)->GetDC();
    HDC hDC=pDC->GetSafeHdc();
    CRect rect;
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect);
    SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
    CvvImage cimg;

    g_srcImage = cv::Mat(TheImage);
    if (g_srcImage.empty())
	{
		MessageBox(_T("δ����ͼ��"),_T("����"),MB_ICONWARNING);
		return;
	}
    namedWindow("��������",WINDOW_AUTOSIZE);
    Mat element = getStructuringElement(MORPH_RECT,Size(2*g_nStructElementSize+1,2*g_nStructElementSize+1),Point(g_nStructElementSize,g_nStructElementSize));
    erode(g_srcImage,g_dstImage,element);//���θ�ʴ
    imshow("��������",g_dstImage);
    //����������
    createTrackbar("��ʴ/����","��������",&g_nTrackbarNumber,1,on_TrackbarNumChange);
    createTrackbar("�ں˳ߴ�","��������",&g_nStructElementSize,21,on_ElementSizeChange);
    on_TrackbarNumChange(g_nTrackbarNumber,0);
    on_ElementSizeChange(g_nStructElementSize,0);

    TheImage = &IplImage(g_dstImage);
    cimg.CopyOf(TheImage);
    cimg.DrawToHDC(hDC,&rect);
    ReleaseDC(pDC);
}

void Copencv3Dlg::OnPicCanny()//Canny��Ե���
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
		MessageBox(_T("δ����ͼ��"),_T("����"),MB_ICONWARNING);
		return;
	}
    cvtColor(g_srcImage,grayImage,CV_BGR2GRAY);
    blur(grayImage,g_dstImage,Size(5,5));
    Canny(g_dstImage,g_dstImage,3,20,3);
    IMG = g_dstImage;//����洢
    IplImage* CannyImg;
    CannyImg = &IplImage(g_dstImage);
    cimg.CopyOf(CannyImg);
    cimg.DrawToHDC(hDC,&rect);
    ReleaseDC(pDC);
}

void Copencv3Dlg::Process()
{
    // TODO: �ڴ���������������
    CDC* pDC=GetDlgItem(IDC_NEW_STATIC)->GetDC();
    HDC hDC=pDC->GetSafeHdc();
    CRect rect;
    GetDlgItem(IDC_NEW_STATIC)->GetClientRect(&rect);
    SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
    CvvImage cimg;   
    //imshow("��������",g_dstImage);
    IMG = imginf;//����洢
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
    imshow("��������",g_dstImage);
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
    imshow("��������",g_dstImage);
    imginf = g_dstImage;
}
////////////////////////////////////////////////////////////////////