
// RobotView.cpp : implementation of the CRobotView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Robot.h"
#endif

#include "RobotDoc.h"
#include "RobotView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD(angle) (angle / 180 * 3.141592)


// CRobotView

IMPLEMENT_DYNCREATE(CRobotView, CView)

BEGIN_MESSAGE_MAP(CRobotView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CRobotView construction/destruction

CRobotView::CRobotView() noexcept
{
	// TODO: add construction code here

	head = new DImage();
	upperArm = new DImage();
	forearm = new DImage();
	thigh = new DImage();
	shin = new DImage();
	hand = new DImage();
	foot = new DImage();
	body = new DImage();
	background = new DImage();

	head->Load(_T("../Res/glava.png"));
	upperArm->Load(_T("../Res/nadlaktica.png"));
	forearm->Load(_T("../Res/podlaktica.png"));
	thigh->Load(_T("../Res/nadkolenica.png"));
	shin->Load(_T("../Res/podkolenica.png"));
	hand->Load(_T("../Res/saka.png"));
	foot->Load(_T("../Res/stopalo.png"));
	body->Load(_T("../Res/telo.png"));
	background->Load(_T("../Res/pozadina.jpg"));

	shoulderBody = { 25, 65 };
	shoulderUpperArm = { 35, 35 };
	elbowUpperArm = { 22, 167 };
	elbowForearm = { 30, 33 };
	wristForearm = { 30, 140 };
	wristHand = { 25, 3 };

	hipBody = { 61, 262 };
	hipLeg = { 29, 20 };
	kneeThigh = { 30, 184 };
	kneeShin = { 25, 37 };
	ankleShin = { 25, 248 };
	ankleFoot = { 20, 16 };

	upperArmAngle = 0;
	forearmAngle = 0;
	handAngle = 0;
	bodyAngle = 0;
	robotScaleFactor = 1;
}

CRobotView::~CRobotView()
{
	delete head;
	delete upperArm;
	delete forearm;
	delete thigh;
	delete shin;
	delete hand;
	delete foot;
	delete body;
	delete background;
}

BOOL CRobotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CRobotView drawing

void CRobotView::OnDraw(CDC* pDC)
{
	CRobotDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CRect cr;
	GetClientRect(cr);

	CBitmap* bmp = new CBitmap;
	bmp->CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());

	CDC* memDC = new CDC;
	memDC->CreateCompatibleDC(pDC);

	memDC->SelectObject(bmp);
	//memDC->FillSolidRect(cr, RGB(255, 255, 255));

	memDC->SetGraphicsMode(GM_ADVANCED);


	DrawBackground(memDC);
	DrawRobot(memDC);


	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), memDC, 0, 0, SRCCOPY);

	delete bmp;
	delete memDC;
}

void CRobotView::DrawBackground(CDC* pDC) {

	CRect imgRect = {
		0,
		0,
		background->Width(),
		background->Height()
	};

	background->Draw(pDC, imgRect, imgRect);
}

void CRobotView::DrawImgTransparent(CDC* pDC, DImage* pImage) {

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


void CRobotView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply) {
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CRobotView::Rotate(CDC* pDC, float angle, bool rightMultiply) {
	double rad = TO_RAD(angle);
	XFORM m = {
		cos(rad), -sin(rad),
		sin(rad), cos(rad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CRobotView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply) {
	XFORM m = {
		sX, 0,
		0, sY,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CRobotView::DrawHalf(CDC* pDC) {

	XFORM old;
	pDC->GetWorldTransform(&old);

	DrawImgTransparent(pDC, body);

	Translate(pDC, shoulderBody.x, shoulderBody.y); 
	Rotate(pDC, upperArmAngle);
	Translate(pDC, -shoulderUpperArm.x, -shoulderUpperArm.y);
	DrawImgTransparent(pDC, upperArm);

	Translate(pDC, elbowUpperArm.x, elbowUpperArm.y);
	Rotate(pDC, forearmAngle);
	Translate(pDC, - elbowForearm.x, - elbowForearm.y);
	DrawImgTransparent(pDC, forearm);

	Translate(pDC, wristForearm.x, wristForearm.y);
	Rotate(pDC, handAngle);
	Translate(pDC,- wristHand.x,- wristHand.y);
	DrawImgTransparent(pDC, hand);

	pDC->SetWorldTransform(&old);
	
	Translate(pDC, hipBody.x - hipLeg.x, hipBody.y - hipLeg.y);
	DrawImgTransparent(pDC, thigh);

	Translate(pDC, kneeThigh.x - kneeShin.x, kneeThigh.y - kneeShin.y);
	DrawImgTransparent(pDC, shin);

	Translate(pDC, ankleShin.x - ankleFoot.x, ankleShin.y - ankleFoot.y);
	DrawImgTransparent(pDC, foot);

	pDC->SetWorldTransform(&old);
}

void  CRobotView::DrawRobot(CDC* pDC) {

	XFORM old;
	pDC->GetWorldTransform(&old);

	Translate(pDC, background->Width()/2, background->Height()/2);
	Rotate(pDC, bodyAngle);

	Scale(pDC, robotScaleFactor, robotScaleFactor);

	Translate(pDC, - body->Width(), - body->Height()*1.1);
	DrawHalf(pDC);
	Translate(pDC, body->Width()*2, 0);
	Scale(pDC, -1, 1);
	DrawHalf(pDC);
	Scale(pDC, -1, 1);
	Translate(pDC, -body->Width() - head->Width()/2, -head->Height() + body->Height() * 0.1);
	DrawImgTransparent(pDC, head);


	pDC->SetWorldTransform(&old);
}

// CRobotView printing

BOOL CRobotView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRobotView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRobotView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CRobotView diagnostics

#ifdef _DEBUG
void CRobotView::AssertValid() const
{
	CView::AssertValid();
}

void CRobotView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRobotDoc* CRobotView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRobotDoc)));
	return (CRobotDoc*)m_pDocument;
}
#endif //_DEBUG


// CRobotView message handlers

void CRobotView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar) {
		case 'a':
		case 'A':
		if (handAngle > -30)
			handAngle -= 10;
		break;
		case 's':
		case 'S':
		if (handAngle < 10)
			handAngle += 10;
		break;
		case 'd':
		case 'D':
		if (forearmAngle > -80)
			forearmAngle -= 10;
		break;
		case 'f':
		case 'F':
		if (forearmAngle < 10)
			forearmAngle += 10;
		break;
		case 'g':
		case 'G':
		if (upperArmAngle > -90)
			upperArmAngle -= 10;
		break;
		case 'h':
		case 'H':
		if (upperArmAngle < 10)
			upperArmAngle += 10;
		break;
		case '1':
			robotScaleFactor -= 0.1;
			bodyAngle -= 10;
		break;
		case '2':
			robotScaleFactor += 0.1;
			bodyAngle += 10;
		break;
	}

	Invalidate(FALSE);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
