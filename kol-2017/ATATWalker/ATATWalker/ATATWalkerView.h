
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

	DImage *body;
	DImage *legPart1;
	DImage *legPart2;
	DImage *legPart3;
	DImage *background;

	CPoint bodyPosition;
	CRect backgroundRect;

	short leg1PosAng;
	short leg2PosAng;
	short leg3PosAng;
	short leg4PosAng;

	bool leg1MovingForward;
	bool leg2MovingForward;
	bool leg3MovingForward;
	bool leg4MovingForward;

	float transporterSizeFactor;

	CPoint frontLegs;
	CPoint backLegs;


// Operations
public:
	void LoadIdentity(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

	void DrawScene(CDC* pDC, CRect rect);
	void DrawLeg(CDC* pDC, double alpha, double dx, double dy);

	void MoveBackground(int dx, int dy);
	void HandleLegMovement(short& legPosAng, bool& movingForward);

	void DrawTransparent(CDC* pDC, DImage* img);

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

