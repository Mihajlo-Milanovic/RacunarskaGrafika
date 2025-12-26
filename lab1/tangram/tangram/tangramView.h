
// tangramView.h : interface of the CtangramView class
//

#pragma once


class CtangramView : public CView
{
protected: // create from serialization only
	CtangramView() noexcept;
	DECLARE_DYNCREATE(CtangramView)

// Attributes
public:
	CtangramDoc* GetDocument() const;
	bool gridEnabled;


// Operations
public:

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
	virtual ~CtangramView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	void drawGrid(CDC* pDC);
	void DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle);

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	
afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in tangramView.cpp
inline CtangramDoc* CtangramView::GetDocument() const
   { return reinterpret_cast<CtangramDoc*>(m_pDocument); }
#endif

