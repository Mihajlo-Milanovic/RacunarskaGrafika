
// Kaktus1View.h : interface of the CKaktus1View class
//

#pragma once


class CKaktus1View : public CView
{
protected: // create from serialization only
	CKaktus1View() noexcept;
	DECLARE_DYNCREATE(CKaktus1View)

// Attributes
public:
	CKaktus1Doc* GetDocument() const;

	bool gridEnabled;
	HENHMETAFILE darkCactus;
	HENHMETAFILE lightCactus;
	int cactusWidth;
	int cactusHeight;
	float angle1;
	float angle2;

// Operations
public:
	void drawGrid(CDC* pDC);
	void drawCactusBranch(CDC* pDC, bool light = false);
	void drawCircle(CDC* pDC);
	void drawPot(CDC* pDC);
	void drawCactus(CDC* pDC);

	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Scale(CDC* pDC, float dX, float dY, bool rightMultiply = false);

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
	virtual ~CKaktus1View();
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
};

#ifndef _DEBUG  // debug version in Kaktus1View.cpp
inline CKaktus1Doc* CKaktus1View::GetDocument() const
   { return reinterpret_cast<CKaktus1Doc*>(m_pDocument); }
#endif

