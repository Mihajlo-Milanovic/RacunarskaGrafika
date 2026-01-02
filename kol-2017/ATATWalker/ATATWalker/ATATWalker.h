
// ATATWalker.h : main header file for the ATATWalker application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CATATWalkerApp:
// See ATATWalker.cpp for the implementation of this class
//

class CATATWalkerApp : public CWinApp
{
public:
	CATATWalkerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CATATWalkerApp theApp;
