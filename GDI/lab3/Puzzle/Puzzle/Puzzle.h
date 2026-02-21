
// Puzzle.h : main header file for the Puzzle application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPuzzleApp:
// See Puzzle.cpp for the implementation of this class
//

class CPuzzleApp : public CWinApp
{
public:
	CPuzzleApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPuzzleApp theApp;
