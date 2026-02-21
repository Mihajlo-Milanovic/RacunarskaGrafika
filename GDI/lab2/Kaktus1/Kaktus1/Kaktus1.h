
// Kaktus1.h : main header file for the Kaktus1 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CKaktus1App:
// See Kaktus1.cpp for the implementation of this class
//

class CKaktus1App : public CWinApp
{
public:
	CKaktus1App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CKaktus1App theApp;
