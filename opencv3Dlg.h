
// opencv3Dlg.h : 头文件
//
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "CvvImage.h"

#pragma once


// Copencv3Dlg 对话框
class Copencv3Dlg : public CDialogEx
{
    
// 构造
public:
	Copencv3Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_OPENCV3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

// 实现
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
// 生成的消息映射函数
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
