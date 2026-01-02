
// Excavator.h : main header file for the Excavator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CExcavatorApp:
// See Excavator.cpp for the implementation of this class
//

class CExcavatorApp : public CWinApp
{
public:
	CExcavatorApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CExcavatorApp theApp;
