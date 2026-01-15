
// TransformerView.cpp : implementation of the CTransformerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Transformer.h"
#endif

#include "TransformerDoc.h"
#include "TransformerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD(angle) (angle / 180 * 3.141592)

// CTransformerView

IMPLEMENT_DYNCREATE(CTransformerView, CView)

BEGIN_MESSAGE_MAP(CTransformerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CTransformerView construction/destruction

CTransformerView::CTransformerView() noexcept
{
	// TODO: add construction code here

	body = new DImage;
	background = new DImage;
	arm1 = new DImage;
	arm2 = new DImage;
	leg1 = new DImage;
	leg2 = new DImage;

	body->Load(_T("body1.png"));
	background->Load(_T("background.jpg"));
	arm1->Load(_T("arm1.png"));
	arm2->Load(_T("arm2.png"));
	leg1->Load(_T("leg1.png"));
	leg2->Load(_T("leg2.png"));

	arm1Angle = 0;
	arm2Angle = 180;
	leg1Angle = 0;
	leg2Angle = 0;
	bodyAngle = 0;

	bodyWaist = { 25, 132};
	bodyShoulder = { 210, 85 };
	arm1Shoulder = { 33, 30 };
	arm1Wrist = { 210, 102 };
	arm2Wrist = { 23, 61 };
	leg1Waist = { 238, 127 };
	leg1Knee = { 28, 126 };
	leg2Knee = { 34, 59 };
}

CTransformerView::~CTransformerView()
{
	delete body;
	delete background;
	delete arm1;
	delete arm2;
	delete leg1;
	delete leg2;
}

BOOL CTransformerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CTransformerView drawing

void CTransformerView::OnDraw(CDC* pDC)
{
	CTransformerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CRect cr;
	GetClientRect(cr);

	CDC* memDC = new CDC;
	memDC->CreateCompatibleDC(pDC);

	CBitmap* bmp = new CBitmap;
	bmp->CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());

	memDC->SelectObject(bmp);
	//memDC->FillSolidRect(cr, RGB(255, 255, 255));

	memDC->SetGraphicsMode(GM_ADVANCED);

	DrawBackground(memDC, cr);

	DrawTransformer(memDC);

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), memDC, 0, 0, SRCCOPY);

	delete bmp;
	delete memDC;
}

void CTransformerView::DrawBackground(CDC *pDC, CRect rc) {
	//background->Draw(pDC, CRect(0, 0, background->Width(), background->Height()), rc);
	background->Draw(pDC, CRect(0, 0, background->Width(), background->Height()), CRect(0, 0, background->Width(), background->Height()));
}

void CTransformerView::DrawImgTransparent(CDC* pDC, DImage* pImage) {

	CBitmap mask;
	mask.CreateBitmap(pImage->Width(), pImage->Height(), 1, 1, NULL);

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

void CTransformerView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply) {
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CTransformerView::Rotate(CDC* pDC, float angle, bool rightMultiply) {
	double rad = TO_RAD(angle);
	XFORM m = {
		cos(rad), -sin(rad),
		sin(rad), cos(rad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CTransformerView::DrawArm1(CDC* pDC) {

	Translate(pDC, arm1Shoulder.x, arm1Shoulder.y);
	Rotate(pDC, arm1Angle);
	Translate(pDC, -arm1Shoulder.x, -arm1Shoulder.y);
	DrawImgTransparent(pDC, arm1);
}

void CTransformerView::DrawArm2(CDC* pDC) {

	Translate(pDC, arm2Wrist.x, arm2Wrist.y);
	Rotate(pDC, arm2Angle);
	Translate(pDC, -arm2Wrist.x, -arm2Wrist.y);
	DrawImgTransparent(pDC, arm2);

}

void CTransformerView::DrawLeg1(CDC* pDC) {

	Translate(pDC, leg1Knee.x, leg1Knee.y);
	Rotate(pDC, leg1Angle);
	Translate(pDC, -leg1Knee.x, -leg1Knee.y);
	DrawImgTransparent(pDC, leg1);

}

void CTransformerView::DrawLeg2(CDC* pDC) {

	Translate(pDC, leg2Knee.x, leg2Knee.y);
	Rotate(pDC, leg2Angle);
	Translate(pDC, -leg2Knee.x, -leg2Knee.y);
	DrawImgTransparent(pDC, leg2);

}

void CTransformerView::DrawBody1(CDC* pDC) {

	Translate(pDC, bodyWaist.x, bodyWaist.y);
	Rotate(pDC, bodyAngle);
	Translate(pDC, -bodyWaist.x, -bodyWaist.y);
	DrawImgTransparent(pDC, body);

}

void CTransformerView::DrawTransformer(CDC* pDC) {

	CRect cr;
	GetClientRect(cr);

	XFORM old, checkpoint;
	pDC->GetWorldTransform(&old);

	Translate(pDC, cr.Width()/2 - body->Width(), cr.Height() / 2);
	DrawLeg1(pDC);

	pDC->GetWorldTransform(&checkpoint);
	
	Translate(pDC, leg1Knee.x - leg2Knee.x, leg1Knee.y - leg2Knee.y);
	DrawLeg2(pDC);

	pDC->SetWorldTransform(&checkpoint);

	Translate(pDC, leg1Waist.x - bodyWaist.x, leg1Waist.y - bodyWaist.y);
	DrawBody1(pDC);

	Translate(pDC, bodyShoulder.x - arm1Shoulder.x, bodyShoulder.y - arm1Shoulder.y);

	pDC->GetWorldTransform(&checkpoint);

	Rotate(pDC, arm1Angle);
	Translate(pDC, arm1Wrist.x - arm2Wrist.x, arm1Wrist.y - arm2Wrist.y);
	DrawArm2(pDC);

	pDC->SetWorldTransform(&checkpoint);

	DrawArm1(pDC);
	

	pDC->SetWorldTransform(&old);
}

// CTransformerView printing

BOOL CTransformerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTransformerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTransformerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CTransformerView diagnostics

#ifdef _DEBUG
void CTransformerView::AssertValid() const
{
	CView::AssertValid();
}

void CTransformerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTransformerDoc* CTransformerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTransformerDoc)));
	return (CTransformerDoc*)m_pDocument;
}
#endif //_DEBUG


// CTransformerView message handlers

void CTransformerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar) {
		case 'Q':
		case 'q':
		arm1Angle += 10;
		break;
		case 'A':
		case 'a':
		arm1Angle -= 10;
		break;
		case 'E':
		case 'e':
		leg1Angle -= 10;
		break;
		case 'D':
		case 'd':
		leg1Angle += 10;
		break;
		case 'T':
		case 't':
		arm2Angle += 10;
		break;
		case 'G':
		case 'g':
		arm2Angle -= 10;
		break;
		case 'R':
		case 'r':
		leg2Angle += 10;
		break;
		case 'F':
		case 'f':
		leg2Angle -= 10;
		break;
		case 'W':
		case 'w':
		bodyAngle += 10;
		break;
		case 'S':
		case 's':
		bodyAngle -= 10;
		break;
		case VK_LEFT:
		if (bodyAngle > -18)
			bodyAngle -= 2;
		if (arm1Angle > -90)
			arm1Angle -= 10;
		if (arm2Angle > 90)
			arm2Angle -= 10;
		if (leg1Angle < 108)
			leg1Angle += 12;
		if (leg2Angle > -216)
			leg2Angle -= 24;
		break;
		
		case VK_RIGHT:
		if (bodyAngle < 0)
			bodyAngle += 2;
		if (arm1Angle < 0)
			arm1Angle += 10;
		if (arm2Angle < 180 )
			arm2Angle += 10;
		if (leg1Angle > 0)
			leg1Angle -= 12;
		if (leg2Angle < 0)
			leg2Angle += 24;
		break;
	}

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
