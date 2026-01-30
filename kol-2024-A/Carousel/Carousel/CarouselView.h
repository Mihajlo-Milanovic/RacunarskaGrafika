
// CarouselView.h : interface of the CCarouselView class
//

#pragma once
#include "DImage.h"


class CCarouselView : public CView
{
protected: // create from serialization only
	CCarouselView() noexcept;
	DECLARE_DYNCREATE(CCarouselView)

// Attributes
public:
	CCarouselDoc* GetDocument() const;

	DImage *background;
	DImage *arm;
	DImage *basket;

	int offset;
	double angle;
	double alpha;
	double beta;

// Operations
public:

	void DrawBackground(CDC* pDC);
	void DrawImageTransparent(CDC* pDC, DImage* pImage);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void DrawArm(CDC* pDC);
	void DrawBasket(CDC* pDC, int r);
	void DrawBasketCouple(CDC* pDC, int l, int r, float angle);
	void DrawPlatform(CDC* pDC, int l, int r, double angle);
	void DrawCarousel(CDC* pDC, int h, int r, double offset, double alpha, double beta, double angle);

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
	virtual ~CCarouselView();
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

#ifndef _DEBUG  // debug version in CarouselView.cpp
inline CCarouselDoc* CCarouselView::GetDocument() const
   { return reinterpret_cast<CCarouselDoc*>(m_pDocument); }
#endif

