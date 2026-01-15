
// ExcavatorView.cpp : implementation of the CExcavatorView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Excavator.h"
#endif

#include "ExcavatorDoc.h"
#include "ExcavatorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD(angle) (angle / 180 * 3.141592)

// CExcavatorView

IMPLEMENT_DYNCREATE(CExcavatorView, CView)

BEGIN_MESSAGE_MAP(CExcavatorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CExcavatorView construction/destruction

CExcavatorView::CExcavatorView() noexcept
{
	// TODO: add construction code here

	excavator = new DImage;
	excavatorArm1 = new DImage;
	excavatorArm2 = new DImage;
	background = new DImage;

	excavator->Load(_T("bager.png"));
	excavatorArm1->Load(_T("arm1.png"));
	excavatorArm2->Load(_T("arm2.png"));
	background->Load(_T("pozadina.png"));

	excavatorFork = GetEnhMetaFile(_T("viljuska.emf"));
	GetEnhMetaFileHeader(excavatorFork, sizeof(ENHMETAHEADER), &excavatorForkHeader);

	excavatorForkRect = {
		0,
		0,
		excavatorForkHeader.rclBounds.right - excavatorForkHeader.rclBounds.left,
		excavatorForkHeader.rclBounds.bottom - excavatorForkHeader.rclBounds.top,
	};

	excavatorForkRect.NormalizeRect();

	arm1Angle = 90;
	arm2Angle = 90;
	forkAngle = 90;
	excavatorDisplacement = 0;

	excavatorArm1Joint = { 63, 171 };
	arm1BigJoint = { 58, 61 };
	arm1SmallJoint = { 309, 61 };
	arm2BigJoint = { 36, 40 };
	arm2SmallJoint = { 260, 40 };
	forkJoint = { 14, 20 };
}

CExcavatorView::~CExcavatorView()
{
	DeleteEnhMetaFile(excavatorFork);

	delete excavator;
	delete excavatorArm1;
	delete excavatorArm2;
	delete background;
}

BOOL CExcavatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CExcavatorView drawing

void CExcavatorView::OnDraw(CDC* pDC)
{
	CExcavatorDoc* pDoc = GetDocument();
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

	memDC->SetGraphicsMode(GM_ADVANCED);

	memDC->SelectObject(bmp);
	//memDC->FillSolidRect(cr, RGB(255, 255, 255));

	DrawBackground(memDC);
	DrawExcavator(memDC);

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), memDC, 0, 0, SRCCOPY);

	delete bmp;
	delete memDC;
}

void CExcavatorView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply) {
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}
void CExcavatorView::Rotate(CDC* pDC, float angle, bool rightMultiply) {
		
	double rad = TO_RAD(angle);
	XFORM m = {
		cos(rad), -sin(rad),
		sin(rad), cos(rad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}
void CExcavatorView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply) {
	XFORM m = {
		sX, 0,
		0, sY,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CExcavatorView::DrawBackground(CDC* pDC) {
	
	CRect backgroundRect(0, 0, background->Width(), background->Height());
	CRect cr, rcRect(backgroundRect);
	GetClientRect(cr);
	rcRect.MoveToXY(cr.Width() / 2 - backgroundRect.Width() / 2, cr.Height() - backgroundRect.Height());

	background->Draw(pDC, backgroundRect, rcRect);
}

void CExcavatorView::DrawImgTransparent(CDC* pDC, DImage* pImage) {
	
	CBitmap mask;
	mask.CreateBitmap(pImage->Width(), pImage->Height(), 1, 1, NULL);

	CDC srcDC, destDC;
	srcDC.CreateCompatibleDC(pDC);
	destDC.CreateCompatibleDC(pDC);

	auto oldSrcBmp = srcDC.SelectObject(pImage->GetBitmap());
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

void CExcavatorView::DrawArm1(CDC* pDC) {

	Translate(pDC, arm1BigJoint.x, arm1BigJoint.y);
	Rotate(pDC, arm1Angle);
	Translate(pDC, -arm1BigJoint.x, -arm1BigJoint.y);
	DrawImgTransparent(pDC, excavatorArm1);

}

void CExcavatorView::DrawArm2(CDC* pDC) {

	Translate(pDC, arm2BigJoint.x, arm2BigJoint.y);
	Rotate(pDC, arm2Angle);
	Translate(pDC, -arm2BigJoint.x, -arm2BigJoint.y);
	DrawImgTransparent(pDC, excavatorArm2);

}

void CExcavatorView::DrawFork(CDC* pDC) {

	Translate(pDC, forkJoint.y, forkJoint.y);
	Rotate(pDC, forkAngle);
	Translate(pDC, -forkJoint.x, -forkJoint.y);
	pDC->PlayMetaFile(excavatorFork, excavatorForkRect);
}

void CExcavatorView::DrawExcavator(CDC* pDC) {
	
	CRect cr;
	GetClientRect(cr);

	XFORM old;
	pDC->GetWorldTransform(&old);

	Translate(pDC, cr.right - excavator->Width() + excavatorDisplacement, cr.bottom - excavator->Height());
	DrawImgTransparent(pDC, excavator);
	
	Translate(pDC, excavatorArm1Joint.x - arm1BigJoint.x, excavatorArm1Joint.y - arm1BigJoint.y);
	DrawArm1(pDC);

	Translate(pDC, arm1SmallJoint.x , arm1SmallJoint.y);
	Translate(pDC, -arm2BigJoint.x, -arm2BigJoint.y);
	DrawArm2(pDC);

	Translate(pDC, arm2SmallJoint.x, arm2SmallJoint.y);
	Scale(pDC, 2.5, 2.5);
	Translate(pDC, -forkJoint.x, -forkJoint.y);
	DrawFork(pDC);


	pDC->SetWorldTransform(&old);
}

// CExcavatorView printing

BOOL CExcavatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CExcavatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CExcavatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CExcavatorView diagnostics

#ifdef _DEBUG
void CExcavatorView::AssertValid() const
{
	CView::AssertValid();
}

void CExcavatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CExcavatorDoc* CExcavatorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CExcavatorDoc)));
	return (CExcavatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CExcavatorView message handlers

void CExcavatorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
		forkAngle += 10;
		break;
		case '6':
		forkAngle -=10;
		break;
		case VK_LEFT:
		excavatorDisplacement -= 10;
		break;
		case VK_RIGHT:
		excavatorDisplacement += 10;
	}
	Invalidate(FALSE);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
