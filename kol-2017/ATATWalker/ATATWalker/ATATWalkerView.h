
// ATATWalkerView.h : interface of the CATATWalkerView class
//

#pragma once
#include "DImage.h"


class CATATWalkerView : public CView
{
protected: // create from serialization only
	CATATWalkerView() noexcept;
	DECLARE_DYNCREATE(CATATWalkerView)

// Attributes
public:
	CATATWalkerDoc* GetDocument() const;

	DImage body;
	DImage legPart1;
	DImage legPart2;
	DImage legPart3;
	DImage back;

	CPoint bodyPosition;
	short* angles;
	short leg1PosAng;
	short leg2PosAng;
	short leg3PosAng;

	float transporterSize;

	CRect cr;
	CRect backgroundRect;

// Operations
public:
	void LoadIdentity(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

	void DrawScene(CDC* pDC, CRect rect);
	void DrawLeg(CDC* pDC, double alpha, double dx, double dy);

	void MoveBackground(int dx, int dy);

	void DrawTransparent(CDC* pDC, DImage &img, double x = 0, double y = 0);

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
	virtual ~CATATWalkerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in ATATWalkerView.cpp
inline CATATWalkerDoc* CATATWalkerView::GetDocument() const
   { return reinterpret_cast<CATATWalkerDoc*>(m_pDocument); }
#endif

