
// ExcavatorView.h : interface of the CExcavatorView class
//

#pragma once
#include "DImage.h"

class CExcavatorView : public CView
{
protected: // create from serialization only
	CExcavatorView() noexcept;
	DECLARE_DYNCREATE(CExcavatorView)

// Attributes
public:
	CExcavatorDoc* GetDocument() const;

	DImage *excavator;
	DImage *excavatorArm1;
	DImage *excavatorArm2;
	DImage *background;

	HENHMETAFILE excavatorFork;
	ENHMETAHEADER excavatorForkHeader;

	CRect excavatorForkRect;

	float arm1Angle;
	float arm2Angle;
	float forkAngle;

	int excavatorDisplacement;

	CPoint excavatorArm1Joint;
	CPoint arm1BigJoint;
	CPoint arm1SmallJoint;
	CPoint arm2BigJoint;
	CPoint arm2SmallJoint;
	CPoint forkJoint;
	

// Operations
public:
void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

void DrawBackground(CDC* pDC);
void DrawImgTransparent(CDC* pDC, DImage* pImage);

void DrawArm1(CDC* pDC);
void DrawArm2(CDC* pDC);
void DrawFork(CDC* pDC);
void DrawExcavator(CDC* pDC);

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
	virtual ~CExcavatorView();
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

#ifndef _DEBUG  // debug version in ExcavatorView.cpp
inline CExcavatorDoc* CExcavatorView::GetDocument() const
   { return reinterpret_cast<CExcavatorDoc*>(m_pDocument); }
#endif

