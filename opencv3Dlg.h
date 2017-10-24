
// opencv3Dlg.h : ͷ�ļ�
//
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "CvvImage.h"

#pragma once


// Copencv3Dlg �Ի���
class Copencv3Dlg : public CDialogEx
{
    
// ����
public:
	Copencv3Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_OPENCV3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

// ʵ��
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
    afx_msg void OnCancel();
	afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void Process();
    
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedCancel2();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnInflate();
    afx_msg void OnVideoCanny();
    afx_msg void OnPicCanny();
    afx_msg void OnFaceDet();
};
