
// Kaktus1View.cpp : implementation of the CKaktus1View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Kaktus1.h"
#endif

#include "Kaktus1Doc.h"
#include "Kaktus1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.14159

// CKaktus1View

IMPLEMENT_DYNCREATE(CKaktus1View, CView)

BEGIN_MESSAGE_MAP(CKaktus1View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CHAR()
END_MESSAGE_MAP()

// CKaktus1View construction/destruction

CKaktus1View::CKaktus1View() noexcept
{
	// TODO: add construction code here
	gridEnabled = true;
	lightCactus = GetEnhMetaFile(_T("..\\cactus_part_light.emf"));
	darkCactus = GetEnhMetaFile(_T("..\\cactus_part.emf"));
	
	angle1 = 0.0;
	angle2 = 0.0;

	ENHMETAHEADER emfHeader;
	GetEnhMetaFileHeader(darkCactus, sizeof(ENHMETAHEADER), &emfHeader);

	cactusWidth = emfHeader.rclBounds.right - emfHeader.rclBounds.left;
	cactusHeight = emfHeader.rclBounds.bottom - emfHeader.rclBounds.top;
}

CKaktus1View::~CKaktus1View()
{
	DeleteEnhMetaFile(lightCactus);
	DeleteEnhMetaFile(darkCactus);
}

BOOL CKaktus1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKaktus1View drawing

void CKaktus1View::OnDraw(CDC* pDC)
{
	CKaktus1Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	auto oldGM = pDC->SetGraphicsMode(GM_ADVANCED);
	
	CBrush blueBrush(RGB(50, 150, 255));
	auto oldBrush = pDC->SelectObject(blueBrush);
	pDC->Rectangle(0, 0, 500, 500);
	pDC->SelectObject(oldBrush);
	
	drawCactus(pDC);

	drawPot(pDC);

	if(gridEnabled)
		drawGrid(pDC);

	pDC->SetGraphicsMode(oldGM);
}

void CKaktus1View::drawGrid(CDC* pDC) {

	CPen gridPen(PS_COSMETIC, -1, RGB(155, 155, 155));
	CPen *oldPen = pDC->SelectObject(&gridPen);

	for (int i = 0; i <= 500; i+=25) {

		pDC->MoveTo(i, 0);
		pDC->LineTo(i, 500);

		pDC->MoveTo(0, i);
		pDC->LineTo(500, i);
	}

	pDC->SelectObject(oldPen);
}

void CKaktus1View::drawCactusBranch(CDC* pDC, bool light) {
	pDC->PlayMetaFile(light ? lightCactus : darkCactus, CRect(-cactusWidth/2, 0, cactusWidth/2, -cactusHeight));
}

void CKaktus1View::drawCircle(CDC* pDC) {
	pDC->Ellipse(-10, -10, 10, 10);
}

void CKaktus1View::drawPot(CDC* pDC) {

	CPen potPen(PS_GEOMETRIC, 1, RGB(0, 0, 0));
	CPen* oldPen = pDC->SelectObject(&potPen);

	CBrush potBrush(RGB(200, 120, 80));
	CBrush* oldBrush = pDC->SelectObject(&potBrush);

	POINT p[4] = { {200, 450}, {300, 450}, {290, 500}, {210, 500} };

	pDC->Polygon(p, 4);

	pDC->Rectangle(188, 430, 312, 450);


	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CKaktus1View::drawCactus(CDC* pDC) {

	CBrush greenBrush(RGB(60, 180, 70));
	CBrush* oldBrush = pDC->SelectObject(&greenBrush);
	XFORM oldWT;
	pDC->GetWorldTransform(&oldWT);

	Translate(pDC, 250, 425);
	drawCircle(pDC);

	Rotate(pDC, angle1);
	drawCactusBranch(pDC, true);

	Translate(pDC, 0, -75);
	drawCircle(pDC);
	
	drawCactusBranch(pDC, false);
	
	Rotate(pDC, -45);
	drawCactusBranch(pDC, false); 

	Rotate(pDC, 90);
	drawCactusBranch(pDC, false);
	Rotate(pDC, -45);

	Translate(pDC, -53, -53);
	drawCircle(pDC);
	drawCactusBranch(pDC, false);

	Rotate(pDC, -90);
	drawCactusBranch(pDC, false);
	Rotate(pDC, 90);

	Translate(pDC, 0, -75);
	drawCircle(pDC);
	drawCactusBranch(pDC, false);

	Translate(pDC, 106, 75);
	drawCircle(pDC);

	Rotate(pDC, angle2);
	drawCactusBranch(pDC, true);

	Rotate(pDC, -angle2 + 90);
	
	drawCactusBranch(pDC, false);
	Translate(pDC, 0, -75);
	drawCircle(pDC);

	Rotate(pDC, 45);
	drawCactusBranch(pDC, false);

	Rotate(pDC, -90);
	drawCactusBranch(pDC, false);
	

	pDC->SetWorldTransform(&oldWT);
	pDC->SelectObject(oldBrush);
}

void CKaktus1View::Rotate(CDC* pDC, float angle, bool rightMultiply) {
	
	float rad = angle * PI / 180;
	float s = sin(rad);
	float c = cos(rad);
	XFORM tm = {
		c,	s,
		-s, c,
		0,	0
	};

	pDC->ModifyWorldTransform(&tm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKaktus1View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply) {

	XFORM tm = {
		1, 0,
		0, 1,
		dX, dY
	};

	pDC->ModifyWorldTransform(&tm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CKaktus1View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply) {

	XFORM tm = {
		sX, 0,
		0, sY,
		0, 0
	};

	pDC->ModifyWorldTransform(&tm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

// CKaktus1View printing

BOOL CKaktus1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKaktus1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKaktus1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKaktus1View diagnostics

#ifdef _DEBUG
void CKaktus1View::AssertValid() const
{
	CView::AssertValid();
}

void CKaktus1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKaktus1Doc* CKaktus1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKaktus1Doc)));
	return (CKaktus1Doc*)m_pDocument;
}
#endif //_DEBUG


// CKaktus1View message handlers

void CKaktus1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar) {
		case ' ':
		gridEnabled = !gridEnabled;
		Invalidate();
		break;

		case 'q':
		angle1 -= 5;
		Invalidate();
		break;

		case 'e':
		angle1 += 5;
		Invalidate();
		break;

		case 'a':
		angle2 -= 5;
		Invalidate();
		break;

		case 'd':
		angle2 += 5;
		Invalidate();
		break;
	}

	CView::OnChar(nChar, nRepCnt, nFlags);
}
