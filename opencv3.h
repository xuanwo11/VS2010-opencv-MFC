
// opencv3.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "CvvImage.h"

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Copencv3App:
// �йش����ʵ�֣������ opencv3.cpp
//

class Copencv3App : public CWinApp
{
public:
	Copencv3App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Copencv3App theApp;