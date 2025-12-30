
// BilliardsView.h : interface of the CBilliardsView class
//

#pragma once
#include "DImage.h"


class CBilliardsView : public CView
{
protected: // create from serialization only
	CBilliardsView() noexcept;
	DECLARE_DYNCREATE(CBilliardsView)

// Attributes
public:
	CBilliardsDoc* GetDocument() const;

	DImage felt;
	DImage wood;

	CPoint ballPosition;
	int ballDiameter;
	float poolCueAngle;
	float ballPathAngle;
	int poolCueDistance;
	int ballDistanceFromStart;

// Operations
public:

	void DrawTable(CDC* pDC, CRect rect);
	void DrawBorder(CDC* pDC, CRect rect, int w);
	void DrawHoles(CDC* pDC, CRect rect, int size);
	void DrawStick(CDC* pDC, int w);
	void DrawBall(CDC* pDC, int w);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);

	void HandleUpArrow();
	void HandleDownArrow();

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
	virtual ~CBilliardsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	public:
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in BilliardsView.cpp
inline CBilliardsDoc* CBilliardsView::GetDocument() const
   { return reinterpret_cast<CBilliardsDoc*>(m_pDocument); }
#endif

