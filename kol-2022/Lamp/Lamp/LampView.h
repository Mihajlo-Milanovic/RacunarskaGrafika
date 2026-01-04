
// LampView.h : interface of the CLampView class
//

#pragma once
#include "DImage.h"

class CLampView : public CView
{
protected: // create from serialization only
	CLampView() noexcept;
	DECLARE_DYNCREATE(CLampView)

// Attributes
public:
	CLampDoc* GetDocument() const;

	DImage base;
	DImage arm1;
	DImage arm2;
	DImage head;
	DImage baseShadow;
	DImage arm1Shadow;
	DImage arm2Shadow;
	DImage headShadow;
	DImage background;

	float arm1Angle;
	float arm2Angle;
	float headAngle;

	CPoint baseJoint;
	CPoint arm1BigJoint;
	CPoint arm1SmallJoint;
	CPoint arm2BigJoint;
	CPoint arm2SmallJoint;
	CPoint headJoint;


// Operations
public:
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

	void DrawBackground(CDC* pDC);
	void DrawImgTransparent(CDC* pDC, DImage* pImage);
	void DrawLampBase(CDC* pDC, bool bIsShadow);
	void DrawLampArm1(CDC* pDC, bool bIsShadow);
	void DrawLampArm2(CDC* pDC, bool bIsShadow);
	void DrawLampHead(CDC* pDC, bool bIsShadow);
	void DrawLamp(CDC* pDC, bool bIsShadow);
	void DrawLampShadow(CDC* pDC);



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
	virtual ~CLampView();
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

#ifndef _DEBUG  // debug version in LampView.cpp
inline CLampDoc* CLampView::GetDocument() const
   { return reinterpret_cast<CLampDoc*>(m_pDocument); }
#endif

