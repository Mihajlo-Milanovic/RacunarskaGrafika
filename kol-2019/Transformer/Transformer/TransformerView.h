
// TransformerView.h : interface of the CTransformerView class
//

#pragma once
#include "DImage.h"

class CTransformerView : public CView
{
protected: // create from serialization only
	CTransformerView() noexcept;
	DECLARE_DYNCREATE(CTransformerView)

// Attributes
public:
	CTransformerDoc* GetDocument() const;

	DImage body;
	DImage background;
	DImage arm1;
	DImage arm2;
	DImage leg1;
	DImage leg2;

	float arm1Angle;
	float arm2Angle;
	float leg1Angle;
	float leg2Angle;
	float bodyAngle;

	CPoint transformerPosition;
	CPoint bodyWaist;
	CPoint bodyShoulder;
	CPoint arm1Shoulder;
	CPoint arm1Wrist;
	CPoint arm2Wrist;
	CPoint leg1Waist;
	CPoint leg1Knee;
	CPoint leg2Knee;

// Operations
public:

	void DrawBackground(CDC* pDC, CRect rc);
	void DrawImgTransparent(CDC* pDC, DImage* pImage);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);

	void DrawArm1(CDC* pDC);
	void DrawArm2(CDC* pDC);
	void DrawLeg1(CDC* pDC);
	void DrawLeg2(CDC* pDC);
	void DrawBody1(CDC* pDC);
	void DrawTransformer(CDC* pDC);

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
	virtual ~CTransformerView();
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

#ifndef _DEBUG  // debug version in TransformerView.cpp
inline CTransformerDoc* CTransformerView::GetDocument() const
   { return reinterpret_cast<CTransformerDoc*>(m_pDocument); }
#endif

