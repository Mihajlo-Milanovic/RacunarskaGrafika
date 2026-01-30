
// Carousel.h : main header file for the Carousel application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CCarouselApp:
// See Carousel.cpp for the implementation of this class
//

class CCarouselApp : public CWinApp
{
public:
	CCarouselApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCarouselApp theApp;
