
// PuzzleView.h : interface of the CPuzzleView class
//

#pragma once
#include "DImage.h"


class CPuzzleView : public CView
{
protected: // create from serialization only
	CPuzzleView() noexcept;
	DECLARE_DYNCREATE(CPuzzleView)

// Attributes
public:
	CPuzzleDoc* GetDocument() const;
	
	DImage *pieces;
	CRect *positions;
	CPoint lastMousePosition;
	float* rotations;
	int imgW;
	int imgH;
	int selectedPiece;
	bool holdingAPiece;
	bool firstDraw;

// Operations
public:
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply);

	void DrawTransparent(CDC* pDC, DImage &img, LONG left, LONG top);
	void DrawPuzzle(CDC* pDC);

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
	virtual ~CPuzzleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in PuzzleView.cpp
inline CPuzzleDoc* CPuzzleView::GetDocument() const
   { return reinterpret_cast<CPuzzleDoc*>(m_pDocument); }
#endif

