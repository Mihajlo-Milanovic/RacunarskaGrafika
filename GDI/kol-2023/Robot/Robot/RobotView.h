
// RobotView.h : interface of the CRobotView class
//

#pragma once
#include "DImage.h"

class CRobotView : public CView
{
protected: // create from serialization only
	CRobotView() noexcept;
	DECLARE_DYNCREATE(CRobotView)

// Attributes
public:
	CRobotDoc* GetDocument() const;

	DImage *head;
	DImage *upperArm;
	DImage *forearm;
	DImage *thigh;
	DImage *shin;
	DImage *hand;
	DImage *foot;
	DImage *body;
	DImage *background;

	CPoint shoulderBody;
	CPoint shoulderUpperArm;
	CPoint elbowUpperArm;
	CPoint elbowForearm;
	CPoint wristForearm;
	CPoint wristHand;

	CPoint hipBody;
	CPoint hipLeg;
	CPoint kneeThigh;
	CPoint kneeShin;
	CPoint ankleShin;
	CPoint ankleFoot;

	float upperArmAngle;
	float forearmAngle;
	float handAngle;
	float bodyAngle;
	float robotScaleFactor;

// Operations
public:

	void DrawBackground(CDC* pDC);
	void DrawImgTransparent(CDC* pDC, DImage* pImage);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

	void DrawHalf(CDC* pDC);
	void DrawRobot(CDC* pDC);

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
	virtual ~CRobotView();
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

#ifndef _DEBUG  // debug version in RobotView.cpp
inline CRobotDoc* CRobotView::GetDocument() const
   { return reinterpret_cast<CRobotDoc*>(m_pDocument); }
#endif

