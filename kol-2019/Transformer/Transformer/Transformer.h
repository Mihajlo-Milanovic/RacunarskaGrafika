
// Transformer.h : main header file for the Transformer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTransformerApp:
// See Transformer.cpp for the implementation of this class
//

class CTransformerApp : public CWinApp
{
public:
	CTransformerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTransformerApp theApp;
