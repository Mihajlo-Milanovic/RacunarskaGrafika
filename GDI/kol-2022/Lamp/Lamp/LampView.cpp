
// LampView.cpp : implementation of the CLampView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Lamp.h"
#endif

#include "LampDoc.h"
#include "LampView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD(angle) (angle / 180 * 3.141592)
 
// CLampView

IMPLEMENT_DYNCREATE(CLampView, CView)

BEGIN_MESSAGE_MAP(CLampView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CLampView construction/destruction

CLampView::CLampView() noexcept
{
	// TODO: add construction code here

	base = new DImage;
	arm1 = new DImage;
	arm2 = new DImage;
	head = new DImage;
	baseShadow = new DImage;
	arm1Shadow = new DImage;
	arm2Shadow = new DImage;
	headShadow = new DImage;
	background = new DImage;

	base->Load(_T("base.png"));
	arm1->Load(_T("arm1.png"));
	arm2->Load(_T("arm2.png"));
	head->Load(_T("head.png"));
	baseShadow->Load(_T("base_shadow.png"));
	arm1Shadow->Load(_T("arm1_shadow.png"));
	arm2Shadow->Load(_T("arm2_shadow.png"));
	headShadow->Load(_T("head_shadow.png"));
	background->Load(_T("pozadina.jpg"));

	arm1Angle = 40;
	arm2Angle = 100;
	headAngle = 180;

	baseJoint = { 167, 38 };
	arm1BigJoint = { 58, 61 };
	arm1SmallJoint = { 309, 61 };
	arm2BigJoint = { 36, 40 };
	arm2SmallJoint = { 272, 40 };
	headJoint = { 179, 101 };
}

CLampView::~CLampView()
{
	delete base;
	delete arm1;
	delete arm2;
	delete head;
	delete baseShadow;
	delete arm1Shadow;
	delete arm2Shadow;
	delete headShadow;
	delete background;

}

BOOL CLampView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLampView drawing

void CLampView::OnDraw(CDC* pDC)
{
	CLampDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CRect cr;
	GetClientRect(cr);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(cr, RGB(255, 255, 255));

	memDC.SetGraphicsMode(GM_ADVANCED);

	XFORM old;
	memDC.GetWorldTransform(&old);

	DrawBackground(&memDC);
	Translate(&memDC, cr.Width()/2 - base->Width()/2, cr.Height() - base->Height()*2);
	DrawLampShadow(&memDC);
	DrawLamp(&memDC, false);

	memDC.SetWorldTransform(&old);
	
	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &memDC, 0, 0, SRCCOPY);

}

void CLampView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply) {
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLampView::Rotate(CDC* pDC, float angle, bool rightMultiply) {
	double rad = TO_RAD(angle);
	XFORM m = {
		cos(rad), -sin(rad),
		sin(rad), cos(rad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLampView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply) {
	XFORM m = {
		sX, 0,
		0, sY,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLampView::DrawBackground(CDC* pDC) {

	CRect cr;
	GetClientRect(cr);
	
	CPoint topLeft(
		-(background->Width() - cr.Width()) / 2,
		-(background->Height() - cr.Height())
	);

	background->Draw(pDC,
					 CRect(
						 0,
						 0,
						 background->Width(),
						 background->Height()
					 ),
					 CRect(
						 topLeft.x,
						 topLeft.y,
						 topLeft.x + background->Width(),
						 topLeft.y + background->Height()
					 )
	);
}

void CLampView::DrawImgTransparent(CDC* pDC, DImage* pImage) {

	CBitmap mask;
	mask.CreateBitmap(pImage->Width(), pImage->Height(), 0, 0, NULL);

	CDC srcDC, destDC;
	srcDC.CreateCompatibleDC(pDC);
	destDC.CreateCompatibleDC(pDC);

	srcDC.SelectObject(pImage->GetBitmap());
	destDC.SelectObject(&mask);

	srcDC.SetBkColor(srcDC.GetPixel(0, 0));
	destDC.BitBlt(0, 0, pImage->Width(), pImage->Height(), &srcDC, 0, 0, SRCCOPY);

	srcDC.SetTextColor(RGB(255, 255, 255));
	srcDC.SetBkColor(RGB(0, 0, 0));
	srcDC.BitBlt(0, 0, pImage->Width(), pImage->Height(), &destDC, 0, 0, SRCAND);
	srcDC.DeleteDC();

	pDC->BitBlt(0, 0, pImage->Width(), pImage->Height(), &destDC, 0, 0, SRCAND);
	destDC.SelectObject(pImage->GetBitmap());
	pDC->BitBlt(0, 0, pImage->Width(), pImage->Height(), &destDC, 0, 0, SRCPAINT);

}

void CLampView::DrawLampBase(CDC* pDC, bool bIsShadow) {

	DrawImgTransparent(pDC, bIsShadow ? baseShadow : base);
	Translate(pDC, baseJoint.x, baseJoint.y);
}

void CLampView::DrawLampArm1(CDC* pDC, bool bIsShadow) {

	Translate(pDC, arm1BigJoint.x, arm1BigJoint.y);
	Rotate(pDC, arm1Angle);
	Translate(pDC, -arm1BigJoint.x, -arm1BigJoint.y);
	DrawImgTransparent(pDC, bIsShadow ? arm1Shadow : arm1);
}

void CLampView::DrawLampArm2(CDC* pDC, bool bIsShadow) {

	Translate(pDC, arm2BigJoint.x, arm2BigJoint.y);
	Rotate(pDC, arm2Angle);
	Translate(pDC, -arm2BigJoint.x, -arm2BigJoint.y);
	DrawImgTransparent(pDC, bIsShadow ? arm2Shadow : arm2);
}

void CLampView::DrawLampHead(CDC* pDC, bool bIsShadow) {

	Translate(pDC, headJoint.x, headJoint.y);
	Rotate(pDC, headAngle);
	Translate(pDC, -headJoint.x, -headJoint.y);
	DrawImgTransparent(pDC, bIsShadow ? headShadow : head);
}

void CLampView::DrawLamp(CDC* pDC, bool bIsShadow) {

	XFORM old, cp;
	pDC->GetWorldTransform(&old);

	DrawLampBase(pDC, bIsShadow);

	Translate(pDC, -arm1BigJoint.x, -arm1BigJoint.y);
	DrawLampArm1(pDC, bIsShadow);

	Translate(pDC, arm1SmallJoint.x, arm1SmallJoint.y);

	pDC->GetWorldTransform(&cp);

	Rotate(pDC, arm2Angle);
	Translate(pDC, arm2SmallJoint.x - arm2BigJoint.x - headJoint.x, arm2SmallJoint.y - arm2BigJoint.y - headJoint.y);
	DrawLampHead(pDC, bIsShadow);

	pDC->SetWorldTransform(&cp);

	Translate(pDC, -arm2BigJoint.x, -arm2BigJoint.y);
	DrawLampArm2(pDC, bIsShadow);

	pDC->SetWorldTransform(&old);
}

void CLampView::DrawLampShadow(CDC* pDC) {

	XFORM old;
	pDC->GetWorldTransform(&old);

	Scale(pDC, 1, 0.25);
	Translate(pDC, base->Width(), base->Height());
	Rotate(pDC, -90);
	DrawLamp(pDC, true);

	pDC->SetWorldTransform(&old);
}

// CLampView printing

BOOL CLampView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLampView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLampView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CLampView diagnostics

#ifdef _DEBUG
void CLampView::AssertValid() const
{
	CView::AssertValid();
}

void CLampView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLampDoc* CLampView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLampDoc)));
	return (CLampDoc*)m_pDocument;
}
#endif //_DEBUG


// CLampView message handlers

void CLampView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar) {
		case '1':
		arm1Angle += 10;
		break;
		case '2':
		arm1Angle -= 10;
		break;
		case '3':
		arm2Angle += 10;
		break;
		case '4':
		arm2Angle -= 10;
		break;
		case '5':
		headAngle += 10;
		break;
		case '6':
		headAngle -= 10;
		break;
	}

	Invalidate(FALSE);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
