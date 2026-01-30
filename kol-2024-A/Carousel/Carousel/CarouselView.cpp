
// CarouselView.cpp : implementation of the CCarouselView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Carousel.h"
#endif

#include "CarouselDoc.h"
#include "CarouselView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.14159265358979323846
#define TO_RAD(angle) (angle * PI / 180.0)
#define AXLE_DIFF 182
#define BASKET_RADIUS 50


// CCarouselView

IMPLEMENT_DYNCREATE(CCarouselView, CView)

BEGIN_MESSAGE_MAP(CCarouselView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CCarouselView construction/destruction

CCarouselView::CCarouselView() noexcept
{
	// TODO: add construction code here

	background = new DImage();
	arm = new DImage();
	basket = new DImage();

	background->Load(_T("../Res/Background.jpg"));
	arm->Load(_T("../Res/Arm.png"));
	basket->Load(_T("../Res/Basket.png"));

	offset = 0;
	angle = 0;
	alpha = 0;
	beta = 0;

}

CCarouselView::~CCarouselView()
{
	delete background;
	delete arm;
	delete basket;
}

BOOL CCarouselView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CCarouselView drawing

void CCarouselView::OnDraw(CDC* pDC)
{
	CCarouselDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CRect cr;
	GetClientRect(&cr);

	CBitmap* bmp = new CBitmap();
	bmp->CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);
	memDC->SelectObject(bmp);

	DrawBackground(memDC);

	memDC->SetGraphicsMode(GM_ADVANCED);

	Translate(memDC, cr.Width()/2, cr.Height() - BASKET_RADIUS * 2, false);
	DrawCarousel(memDC, AXLE_DIFF, BASKET_RADIUS, offset, alpha, beta, angle);
	Translate(memDC, -cr.Width() / 2, -(cr.Height() - BASKET_RADIUS * 2), false);
	


	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), memDC, 0, 0, SRCCOPY);

	delete memDC;
	delete bmp;
}

void CCarouselView::DrawBackground(CDC* pDC) {

	CRect cr;
	GetClientRect(&cr);

	CRect imgRect(0, 0, background->Width(), background->Height());
	int x = (cr.Width() - imgRect.Width()) / 2;
	int y = cr.Height() - imgRect.Height();

	background->Draw(pDC, imgRect, CRect(x, y, imgRect.Width() + x, imgRect.Height() + y));
}

void CCarouselView::DrawImageTransparent(CDC* pDC, DImage* pImage) {

	CRect imgRect(0, 0, pImage->Width(), pImage->Height());

	RGBQUAD* pPixels = (RGBQUAD*)pImage->GetDIBBits();

	COLORREF clrRef = RGB(pPixels[0].rgbRed,
						  pPixels[0].rgbGreen,	
						  pPixels[0].rgbBlue	
	);
	
	pImage->DrawTransparent(pDC, imgRect, imgRect, clrRef);
}

void CCarouselView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply){
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CCarouselView::Rotate(CDC* pDC, float angle, bool rightMultiply){
	
	double rad = TO_RAD(angle);
	XFORM m = {
		cos(rad), -sin(rad),
		sin(rad), cos(rad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CCarouselView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply){
	XFORM m = {
		sX, 0,
		0, sY,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CCarouselView::DrawArm(CDC* pDC) {

	Translate(pDC, -10, -10, false);
	DrawImageTransparent(pDC, arm);
	Translate(pDC, 10, 10, false);
}

void CCarouselView::DrawBasket(CDC* pDC, int r) {

	XFORM old;
	pDC->GetWorldTransform(&old);

	CFont font;
	font.CreateFontW(0.9 * r, 0, 300, 0, FW_BOLD,
					 FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
					 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
					 FIXED_PITCH | FF_SWISS | TA_CENTER | TA_BASELINE, _T("Verdana")
	);

	Scale(pDC, 0.675, 0.675, false);
	Translate(pDC, -basket->Width() / 2, -basket->Height()/2, false);
	DrawImageTransparent(pDC, basket);
	Scale(pDC, 1 / 0.675, 1 / 0.675, false);
	

	auto oldFont = pDC->SelectObject(&font);
	auto oldTextColor = pDC->SetTextColor(RGB(0, 0, 155));
	auto oldBkMode = pDC->SetBkMode(TRANSPARENT);
	auto oldGM = pDC->SetGraphicsMode(GM_COMPATIBLE);

	CSize textSize = pDC->GetTextExtent(_T("R_G"));
	pDC->TextOut(5, textSize.cy, _T("R_G"), 3);

	pDC->SetGraphicsMode(oldGM);
	pDC->SetBkMode(oldBkMode);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);

	pDC->SetWorldTransform(&old);
}

void CCarouselView::DrawBasketCouple(CDC* pDC, int l, int r, float angle) {

	XFORM old;
	pDC->GetWorldTransform(&old);

	Rotate(pDC, angle, false);
	DrawBasket(pDC, r);

	pDC->SetWorldTransform(&old);

	Translate(pDC, l, 0, false);
	Rotate(pDC, angle, false);
	DrawBasket(pDC, r);

	pDC->SetWorldTransform(&old);

	Translate(pDC, 0.8 * r, 0, false);
	Rotate(pDC, 90, false);
	DrawArm(pDC);

	pDC->SetWorldTransform(&old);
}

void CCarouselView::DrawPlatform(CDC* pDC, int l, int r, double angle) {

	XFORM old, cp;
	pDC->GetWorldTransform(&old);

	Rotate(pDC, angle, false);
	DrawBasket(pDC, r);

	pDC->GetWorldTransform(&cp);

	for (int i = 0; i < 360; i+=60) {
		Rotate(pDC, i, false);
		Translate(pDC, BASKET_RADIUS*2, 0, false);
		DrawBasketCouple(pDC, l, r, -angle);
		pDC->SetWorldTransform(&cp);
	}

	pDC->SetWorldTransform(&old);

}

void CCarouselView::DrawCarousel(CDC* pDC, int h, int r, double offset, double alpha, double beta, double angle) {

	XFORM old;
	pDC->GetWorldTransform(&old);

	Translate(pDC, offset, 0, false);
	Rotate(pDC, -2 * offset * PI, false);
	DrawBasket(pDC, r);

	Rotate(pDC, alpha + 2 * offset * PI + 180, false);
	DrawArm(pDC);

	Translate(pDC, 0, h, false);
	Rotate(pDC, beta, false);
	DrawArm(pDC);


	Translate(pDC, 0, h, false);
	DrawPlatform(pDC, h, r, angle+180);
	
	pDC->SetWorldTransform(&old);
}

// CCarouselView printing

BOOL CCarouselView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCarouselView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCarouselView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CCarouselView diagnostics

#ifdef _DEBUG
void CCarouselView::AssertValid() const
{
	CView::AssertValid();
}

void CCarouselView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCarouselDoc* CCarouselView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCarouselDoc)));
	return (CCarouselDoc*)m_pDocument;
}
#endif //_DEBUG


// CCarouselView message handlers

void CCarouselView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar) {
		case 'Q':
		case 'q':
			offset -= 5;
			break;
		case 'W':
		case 'w':
			offset += 5;
			break;
		case 'E':
		case 'e':
			angle += 5;
			break;
		case 'R':
		case 'r':
			angle -= 5;
			break;
		case 'A':
		case 'a':
			alpha += 5;
			break;
		case 'S':
		case 's':
			alpha -= 5;
			break;
		case 'D':
		case 'd':
			beta += 5;
			break;
		case 'F':
		case 'f':
			beta -= 5;
			break;
	}

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
