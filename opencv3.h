
// opencv3.h : PROJECT_NAME 应用程序的主头文件
//
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "CvvImage.h"

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// Copencv3App:
// 有关此类的实现，请参阅 opencv3.cpp
//

class Copencv3App : public CWinApp
{
public:
	Copencv3App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern Copencv3App theApp;