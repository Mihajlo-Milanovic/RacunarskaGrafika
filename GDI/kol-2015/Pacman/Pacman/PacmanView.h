
// PacmanView.h : interface of the CPacmanView class
//

#pragma once
#include "DImage.h"


class CPacmanView : public CView
{
protected: // create from serialization only
	CPacmanView() noexcept;
	DECLARE_DYNCREATE(CPacmanView)

// Attributes
public:
	CPacmanDoc* GetDocument() const;

	DImage *background;

	float mouthAngle;
	float pacmanAngle;

	bool mirrorPacman;
	bool gameOver;

	CRect pacmanPosition;
	CRect ghostPosition;


// Operations
public:
	void Translate(CDC* pDC, int dX, int dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

	void DrawPacman(CDC* pDC, CRect rect, float angle);
	void DrawGhost(CDC* pDC, CRect rect);
	void DrawGhostEye(CDC* pDC, CPoint center, int eyeRadius, int irisRadius);

	void DetectCollision();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CPacmanView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	public:
//	afx_msg void OnDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in PacmanView.cpp
inline CPacmanDoc* CPacmanView::GetDocument() const
   { return reinterpret_cast<CPacmanDoc*>(m_pDocument); }
#endif

