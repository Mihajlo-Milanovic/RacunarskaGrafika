
// BilliardsView.cpp : implementation of the CBilliardsView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Billiards.h"
#endif

#include "BilliardsDoc.h"
#include "BilliardsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD(angle) (angle / 180 * 3.14)

// CBilliardsView

IMPLEMENT_DYNCREATE(CBilliardsView, CView)

BEGIN_MESSAGE_MAP(CBilliardsView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CBilliardsView construction/destruction

CBilliardsView::CBilliardsView() noexcept
{
	// TODO: add construction code here

	felt = new DImage;
	wood = new DImage;

	felt->Load(_T("felt2.jpg"));
	wood->Load(_T("wood.jpg"));

	ballDiameter = 30;
	ballPosition = {600, 400};
	ballDistanceFromStart = 0;

	poolCueAngle = -45.0;
	poolCueDistance = ballDiameter;

	ballPathAngle = poolCueAngle;
}

CBilliardsView::~CBilliardsView()
{
	delete felt;
	delete wood;
}

BOOL CBilliardsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CBilliardsView drawing

void CBilliardsView::OnDraw(CDC* pDC)
{
	CBilliardsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CRect cr;
	GetClientRect(cr);
	
	CDC* memDC = new CDC;
	memDC->CreateCompatibleDC(pDC);
	CBitmap *bmp = new CBitmap;
	bmp->CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());

	memDC->SelectObject(bmp);
	//memDC->FillSolidRect(cr, RGB(255, 255, 255));

	memDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldWT, wt;
	memDC->GetWorldTransform(&oldWT);

	CRect perfectSizeRect(0, 0, felt->Width() * 4 + 100, felt->Height() * 2 + 100); // because of felt dimensions

	DrawBorder(memDC, /*cr*/perfectSizeRect, 50);

	Translate(memDC, ballPosition.x, ballPosition.y);

	memDC->GetWorldTransform(&wt);

	Rotate(memDC, ballPathAngle);
	Translate(memDC, 0, -ballDistanceFromStart);
	DrawBall(memDC, ballDiameter);

	memDC->SetWorldTransform(&oldWT);

	DrawHoles(memDC, /*cr*/perfectSizeRect, 50);

	memDC->SetWorldTransform(&wt);

	Rotate(memDC, poolCueAngle);
	Translate(memDC, 0, poolCueDistance);
	Translate(memDC, 0, poolCueDistance);
	DrawStick(memDC, 500);

	memDC->SetWorldTransform(&oldWT);

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), memDC, 0, 0, SRCCOPY);
}

void CBilliardsView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply) {
	XFORM m{
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CBilliardsView::Rotate(CDC* pDC, float angle, bool rightMultiply) {
	float rad = TO_RAD(angle);
	XFORM m{
		cos(rad), -sin(rad),
		sin(rad), cos(rad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}


void CBilliardsView::DrawTable(CDC* pDC, CRect rect) {

	int i = rect.left, j = rect.top;
	int iCount = 4;//rect.Width() / felt.Width();
	int jCount = 2;//rect.Height() / felt.Height();

	for (int iCounter = 0; iCounter < iCount; iCounter++) {
		for (int jCounter = 0; jCounter < jCount; jCounter++) {
			felt->Draw(pDC, CRect(0, 0, felt->Width(), felt->Height()), CRect(i, j, i + felt->Width(), j + felt->Height()));
			j += felt->Height();
		}
		j = rect.top;
		i += felt->Width();
	}

}

void CBilliardsView::DrawBorder(CDC* pDC, CRect rect, int w) {

	wood->Draw(pDC, CRect(0, 0, wood->Width(), wood->Height()), rect);

	DrawTable(pDC, CRect(rect.left + w, rect.top + w, rect.right - w, rect.bottom - w));
}

void CBilliardsView::DrawHoles(CDC* pDC, CRect rect, int size) {

	int radius = size / 2;

	CPoint holePos[6] = {
		CPoint(rect.left + size, rect.top + size),
		CPoint(rect.left + size, rect.bottom - size),

		CPoint(rect.CenterPoint().x, rect.top + size),
		CPoint(rect.CenterPoint().x, rect.bottom - size),

		CPoint(rect.right - size, rect.top + size),
		CPoint(rect.right - size, rect.bottom - size),
	};

	CPen blackPen(PS_COSMETIC, 1, RGB(0, 0, 0));
	CBrush blackBrush(RGB(0, 0, 0));
	
	auto oldPen = pDC->SelectObject(&blackPen);
	auto oldBrush = pDC->SelectObject(&blackBrush);

	for (auto h : holePos) {
		pDC->Ellipse(h.x - radius, h.y - radius, h.x + radius, h.y + radius);
	}

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CBilliardsView::DrawStick(CDC* pDC, int w) {

	int top = w / 200;
	int bottom = w / 100;

	CPoint trapezoid[6] = {
		CPoint(-top, 0),
		CPoint(top, 0),
		CPoint((top + bottom) * 2/3, w * 2/3),
		CPoint(-(top + bottom) * 2 / 3, w * 2 / 3),
		CPoint(-bottom, w),
		CPoint(bottom, w),
	};

	CPen brownPen(PS_COSMETIC, 1, RGB(51, 11, 0));
	auto oldPen = pDC->SelectObject(&brownPen);

	CBrush ochreBrush(RGB(254, 189, 126));	
	auto oldBrush = pDC->SelectObject(&ochreBrush);

	pDC->Polygon(trapezoid, 4);

	CBrush brownBrush(RGB(63, 31, 0));
	pDC->SelectObject(&brownBrush);
	
	pDC->BeginPath();

	pDC->MoveTo(trapezoid[3]);
	pDC->LineTo(trapezoid[4]);

	pDC->ArcTo(CRect(-bottom, w - bottom, bottom, w + bottom), trapezoid[4], trapezoid[5]);

	pDC->LineTo(trapezoid[5]);
	pDC->LineTo(trapezoid[2]);

	pDC->EndPath();
	pDC->CloseFigure();
	pDC->StrokeAndFillPath();

	CPen whitePen(PS_COSMETIC, 1, RGB(255, 255, 255));
	pDC->SelectObject(&whitePen);

	pDC->MoveTo(CPoint(-top+2, 0));
	pDC->LineTo(CPoint(-bottom + 2, w));

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	
}

void CBilliardsView::DrawBall(CDC* pDC, int w) {

	CPen redPen(PS_COSMETIC, 1, RGB(77, 0, 0));
	CBrush redBrush(RGB(127, 0, 0));

	CPen whitePen(PS_COSMETIC, 1, RGB(255, 255, 255));
	CBrush whiteBrush(RGB(255, 255, 255));

	int ballRadius = w / 2;
	int reflectionRadius = ballRadius / 3;

	auto oldPen = pDC->SelectObject(&redPen);
	auto oldBrush = pDC->SelectObject(&redBrush);

	pDC->Ellipse(CRect(-ballRadius, -ballRadius, ballRadius, ballRadius));

	pDC->SelectObject(&whitePen);
	pDC->SelectObject(&whiteBrush);

	pDC->Ellipse(CRect(-reflectionRadius / 3, -reflectionRadius * 1.67, reflectionRadius * 1.67, reflectionRadius / 3));

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CBilliardsView::HandleUpArrow(){

	if (ballDistanceFromStart == 0 && poolCueDistance > ballDiameter / 2) {
		poolCueDistance -= 5;
		ballPathAngle = poolCueAngle;
	} else
		ballDistanceFromStart += 20;
}

void CBilliardsView::HandleDownArrow() {

	if (ballDistanceFromStart == 0)
		poolCueDistance += 5;
	else
		ballDistanceFromStart -= 20;
}


// CBilliardsView printing

BOOL CBilliardsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBilliardsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBilliardsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CBilliardsView diagnostics

#ifdef _DEBUG
void CBilliardsView::AssertValid() const
{
	CView::AssertValid();
}

void CBilliardsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBilliardsDoc* CBilliardsView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBilliardsDoc)));
	return (CBilliardsDoc*)m_pDocument;
}
#endif //_DEBUG


// CBilliardsView message handlers

UINT CBilliardsView::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

void CBilliardsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar) {
		case VK_UP:
			HandleUpArrow();
			break;
		case VK_DOWN:
			HandleDownArrow();
			break;
		case VK_LEFT:
			poolCueAngle += 1.0;
			break;
		case VK_RIGHT:
			poolCueAngle -= 1.0;
			break;
		case ' ':
			poolCueDistance = ballDiameter;
			ballDistanceFromStart = 0;
			ballPathAngle = poolCueAngle;
			break;

	}

	Invalidate(FALSE);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
