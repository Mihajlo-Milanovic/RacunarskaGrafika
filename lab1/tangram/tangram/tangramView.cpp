
// tangramView.cpp : implementation of the CtangramView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "tangram.h"
#endif

#include "tangramDoc.h"
#include "tangramView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEGREE_TO_RAD 3.14159/180
#define RAD_TO_DEGREE 180/3.14159

// CtangramView

IMPLEMENT_DYNCREATE(CtangramView, CView)

BEGIN_MESSAGE_MAP(CtangramView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
END_MESSAGE_MAP()

// CtangramView construction/destruction

CtangramView::CtangramView() noexcept
{
	// TODO: add construction code here

	gridEnabled = true;

}

CtangramView::~CtangramView()
{
}

BOOL CtangramView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CtangramView drawing

void CtangramView::OnDraw(CDC* pDC)
{
	CtangramDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here


	
	CPen* greenPen = new CPen(PS_SOLID, 4, RGB(0, 255, 0));
	CPen* oldPen = pDC->SelectObject(greenPen);

	CBrush* orangeBrush = new CBrush(RGB(255, 165, 0));
	CBrush* oldBrush = pDC->SelectObject(orangeBrush);

	//HEAD
	POINT trianglePoints[3] = {
		CPoint(320, 22), 
		CPoint(240, 110), 
		CPoint(400, 110)
	};
	pDC->Polygon(trianglePoints, 3);

	DrawRegularPolygon(pDC, 320, 72, 18, 8, 0); ///////////////////////

	//NECK
	CBrush* greenBrush = new CBrush(RGB(0, 200, 0));
	pDC->SelectObject(greenBrush);
	delete orangeBrush;

	DrawRegularPolygon(pDC, 277.5, 150, 55, 4, 45);

	//UPPER BODY
	CBrush* purpuleBrush = new CBrush(RGB(160, 0, 190));
	pDC->SelectObject(purpuleBrush);
	delete greenBrush;

	trianglePoints[0] = CPoint(240, 191);
	trianglePoints[1] = CPoint(400, 191);
	trianglePoints[2] = CPoint(240, 341);
	pDC->Polygon(trianglePoints, 3);

	DrawRegularPolygon(pDC, 287.5, 238, 25, 7, 0); ///////////////////////

	//LOWER BODY
	CBrush* whiteStripesBrush = new CBrush(HS_FDIAGONAL, RGB(0, 0, 255));
	pDC->SelectObject(whiteStripesBrush);
	delete purpuleBrush;

	trianglePoints[0] = CPoint(240, 191);
	trianglePoints[1] = CPoint(240, 416);
	trianglePoints[2] = CPoint(125, 300);
	pDC->Polygon(trianglePoints, 3);

	DrawRegularPolygon(pDC, 187.5, 300, 25, 6, 0); ///////////////////////

	//UPPER LEGS
	CBrush* pinkBrush = new CBrush(RGB(255, 192, 203));
	pDC->SelectObject(pinkBrush);
	delete whiteStripesBrush;

	trianglePoints[0] = CPoint(400, 191);
	trianglePoints[1] = CPoint(340, 251);
	trianglePoints[2] = CPoint(460, 251);
	pDC->Polygon(trianglePoints, 3);

	DrawRegularPolygon(pDC, 400, 225, 15, 5, 0); ///////////////////////


	//LOWER LEGS
	CBrush* yellowBrush = new CBrush(RGB(255, 255, 0));
	pDC->SelectObject(yellowBrush);
	delete pinkBrush;

	trianglePoints[0] = CPoint(240, 390);
	trianglePoints[1] = CPoint(240, 480);
	trianglePoints[2] = CPoint(315, 480);
	pDC->Polygon(trianglePoints, 3);

	DrawRegularPolygon(pDC, 262.5, 450, 15, 4, 0); ///////////////////////

	//TAIL
	CBrush* redBrush = new CBrush(RGB(255, 0, 0));
	pDC->SelectObject(redBrush);
	delete yellowBrush;

	
	POINT tailPoints[4] = {
		CPoint(125, 300),
		CPoint(125, 393.75),
		CPoint(38, 487.5),
		CPoint(38, 393.75)
	};
	pDC->Polygon(tailPoints, 4);

	delete redBrush;

	pDC->SelectObject(oldPen);
	delete greenPen;

	
	if (gridEnabled)
		drawGrid(pDC);

}

void CtangramView::drawGrid(CDC* pDC) {

		CPen *gridPen = new CPen(PS_COSMETIC | PS_SOLID, 0, RGB(199, 199, 199)); // kozmeticka olovka
		CPen *oldPen = pDC->SelectObject(gridPen);

		for (short i = 0; i <= 500; i += 25) {

			pDC->MoveTo(i, 0);
			pDC->LineTo(i, 500);

			pDC->MoveTo(0, i);
			pDC->LineTo(500, i);
		}

		pDC->SelectObject(oldPen);
		delete gridPen;	
}

void CtangramView::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle) {

	double alpha = 360 * DEGREE_TO_RAD / n;
	double startingRotion = rotAngle * DEGREE_TO_RAD;
	POINT *points = new POINT[n];

	for (int i = 0; i < n; i++) {

		points[i].x = cx + r * cos(i * alpha + startingRotion);
		points[i].y = cy + r * sin(i * alpha + startingRotion);

	}
	pDC->MoveTo(cx, cy);
	pDC->Polygon(points, n);

	delete[] points;
}


// CtangramView printing

BOOL CtangramView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CtangramView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CtangramView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CtangramView diagnostics

#ifdef _DEBUG
void CtangramView::AssertValid() const
{
	CView::AssertValid();
}

void CtangramView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CtangramDoc* CtangramView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CtangramDoc)));
	return (CtangramDoc*)m_pDocument;
}
#endif //_DEBUG


// CtangramView message handlers


void CtangramView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'g' || nChar == 'G') {
		gridEnabled = !gridEnabled;
		Invalidate();
	}

	CView::OnChar(nChar, nRepCnt, nFlags);
}
