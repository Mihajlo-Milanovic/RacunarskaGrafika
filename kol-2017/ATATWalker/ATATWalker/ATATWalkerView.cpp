
// ATATWalkerView.cpp : implementation of the CATATWalkerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ATATWalker.h"
#endif

#include "ATATWalkerDoc.h"
#include "ATATWalkerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD(angle) (angle / 180 * 3.14)

// CATATWalkerView

IMPLEMENT_DYNCREATE(CATATWalkerView, CView)

BEGIN_MESSAGE_MAP(CATATWalkerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CATATWalkerView construction/destruction

CATATWalkerView::CATATWalkerView() noexcept
{
	// TODO: add construction code here
	/*Body.png, Leg1.png,
		Leg2.png, Leg3.png i Back2.jpg*/
	body.Load(_T("Body.png"));
	legPart1.Load(_T("Leg1.png"));
	legPart2.Load(_T("Leg2.png"));
	legPart3.Load(_T("Leg3.png"));
	back.Load(_T("Back2.jpg"));
	
	bodyPosition = { 100, 100 };
	angles = new short[8]{ -20, -10, 0, 10, 20, 10, 0, -10};
	leg1PosAng = 2;
	leg2PosAng = 0;
	leg3PosAng = 4;

	transporterSize = 1;
}

CATATWalkerView::~CATATWalkerView()
{
	delete[] angles;
}

BOOL CATATWalkerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CATATWalkerView drawing

void CATATWalkerView::OnDraw(CDC* pDC)
{
	CATATWalkerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	GetClientRect(cr);
	if (backgroundRect.IsRectEmpty())
		backgroundRect.CopyRect(cr);


	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());
	memDC.SelectObject(&bmp);

	memDC.FillSolidRect(cr, RGB(255, 255, 255));

	int oldGM = memDC.SetGraphicsMode(GM_ADVANCED);


	DrawScene(&memDC, cr);

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &memDC, 0, 0, SRCCOPY);
}

void CATATWalkerView::LoadIdentity(CDC* pDC) {
	//?
}

void CATATWalkerView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply) {
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}
void CATATWalkerView::Rotate(CDC* pDC, float angle, bool rightMultiply) {
	
	float rad = TO_RAD(angle);
	XFORM m = {
		cos(rad), -sin(rad),
		sin(rad), cos(rad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}
void CATATWalkerView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply) {
	XFORM m = {
		sX, 0,
		0, sY,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CATATWalkerView::DrawScene(CDC* pDC, CRect rect) {

	back.Draw(pDC, CRect(0, 0, back.Width(), back.Height()), backgroundRect);

	DrawTransparent(pDC, body, bodyPosition.x, bodyPosition.y);

	DrawLeg(pDC, angles[leg1PosAng], bodyPosition.x + 50, bodyPosition.y + 168); // bodyPosition.x + 72 was not centered
	DrawLeg(pDC, angles[leg2PosAng], bodyPosition.x + 270, bodyPosition.y + 168);
	DrawLeg(pDC, angles[leg3PosAng], bodyPosition.x + 270, bodyPosition.y + 168);	
}

void CATATWalkerView::DrawLeg(CDC* pDC, double alpha, double dx, double dy) {

	XFORM oldT;
	pDC->GetWorldTransform(&oldT);

	Rotate(pDC, alpha);
	Translate(pDC, -28 /*-leg1.Width() / 2*/, -28 /*-leg1.Width() / 2*/);
	Translate(pDC, dx + 28 /*leg1.Width() / 2*/, dy + 28 /*leg1.Width() / 2*/, true);
	
	DrawTransparent(pDC, legPart1);

	Translate(pDC, 2, (legPart1.Height() - 40));
	
	Translate(pDC, 29, 29);
	Rotate(pDC, -alpha);
	Translate(pDC, -29, -29);


	DrawTransparent(pDC, legPart2, 0, 0);
	Translate(pDC, -33, legPart2.Height() - 19);
	DrawTransparent(pDC, legPart3);


	pDC->SetWorldTransform(&oldT);
}

void CATATWalkerView::MoveBackground(int dx, int dy) {

	backgroundRect.OffsetRect(dx, dy);

	if (!(backgroundRect.left < cr.left &&
		backgroundRect.top < cr.top &&
		backgroundRect.right > cr.right &&
		backgroundRect.bottom > cr.bottom)
	)
		backgroundRect.OffsetRect(-dx, -dy);
	

}

void CATATWalkerView::DrawTransparent(CDC* pDC, DImage &img, double x, double y) {

	CBitmap mask;
	mask.CreateBitmap(img.Width(), img.Height(), 1, 1, NULL);
	
	CDC srcDC, destDC;
	srcDC.CreateCompatibleDC(pDC);
	destDC.CreateCompatibleDC(pDC);

	auto oldSrcBmp = srcDC.SelectObject(img.GetBitmap());
	auto oldDestBmp = destDC.SelectObject(&mask);

	COLORREF topLeft = srcDC.GetPixel(1, 1);
	COLORREF oldSrcBkClr = srcDC.SetBkColor(topLeft);

	destDC.BitBlt(0, 0, img.Width(), img.Height(), &srcDC, 0, 0, SRCCOPY);

	COLORREF oldSrcTextColor = srcDC.SetTextColor(RGB(255, 255, 255));
	srcDC.SetBkColor(RGB(0, 0, 0));
	srcDC.BitBlt(0, 0, img.Width(), img.Height(), &destDC, 0, 0, SRCAND);
	srcDC.SelectObject(oldSrcBmp);


	pDC->BitBlt(x, y, img.Width(), img.Height(), &destDC, 0, 0, SRCAND);
	destDC.SelectObject(img.GetBitmap());
	pDC->BitBlt(x, y, img.Width(), img.Height(), &destDC, 0, 0, SRCPAINT);

}

// CATATWalkerView printing

BOOL CATATWalkerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CATATWalkerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CATATWalkerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CATATWalkerView diagnostics

#ifdef _DEBUG
void CATATWalkerView::AssertValid() const
{
	CView::AssertValid();
}

void CATATWalkerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CATATWalkerDoc* CATATWalkerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CATATWalkerDoc)));
	return (CATATWalkerDoc*)m_pDocument;
}
#endif //_DEBUG


// CATATWalkerView message handlers

void CATATWalkerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar) {
		/*case VK_LEFT:
	
		break;*/
		case VK_RIGHT:
			leg1PosAng = (leg1PosAng + 1) % 8;
			leg2PosAng = (leg2PosAng + 1) % 8;
			leg3PosAng = (leg3PosAng + 1) % 8;
		break;
		case VK_UP:
			backgroundRect.InflateRect(backgroundRect.Width() * 0.1, backgroundRect.Height() * 0.1);
		break;
		case VK_DOWN:
			backgroundRect.DeflateRect(backgroundRect.Width() * 0.1, backgroundRect.Height() * 0.1);
		break;
		case 'A':
		case 'a':
			MoveBackground(-200, 0);
		break;
		case 'D':
		case 'd':
			MoveBackground(200, 0);
		break;
		case 'W':
		case 'w':
			MoveBackground(0, -200);
		break;
		case 'S':
		case 's':
			MoveBackground(0, 200);
		
		break;

	}

	Invalidate(FALSE);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
