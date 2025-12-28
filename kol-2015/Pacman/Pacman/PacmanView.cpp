
// PacmanView.cpp : implementation of the CPacmanView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Pacman.h"
#endif

#include "PacmanDoc.h"
#include "PacmanView.h"
//#include "DImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD(angle) (angle/180*3.14)

// CPacmanView

IMPLEMENT_DYNCREATE(CPacmanView, CView)

BEGIN_MESSAGE_MAP(CPacmanView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)

//	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPacmanView construction/destruction

CPacmanView::CPacmanView() noexcept
{
	// TODO: add construction code here
	
	background.Load(_T("blue.png"));
	mouthAngle = 30;
	pacmanAngle = 0;
	mirrorPacman = false;
	gameOver = false;
	pacmanPosition = { 400, 400 , 500, 500};
	ghostPosition = { 100, 120, 196, 216 };

}

CPacmanView::~CPacmanView()
{
}

BOOL CPacmanView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CPacmanView drawing

void CPacmanView::OnDraw(CDC* pDC)
{
	CPacmanDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CRect cr;
	GetClientRect(cr);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());

	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(cr, RGB(255, 255, 255));
	
	memDC.SetGraphicsMode(GM_ADVANCED);

	/**
	* 
	*/
	DetectCollision();

	background.Draw(&memDC, CRect(0, 0, background.Width(), background.Height()), cr);


	XFORM oldWT;
	memDC.GetWorldTransform(&oldWT);

	Scale(&memDC, mirrorPacman ? -1 : 1, 1);
	Rotate(&memDC, pacmanAngle);
	Translate(&memDC, - pacmanPosition.CenterPoint().x, - pacmanPosition.CenterPoint().y);
	Translate(&memDC, pacmanPosition.CenterPoint().x, pacmanPosition.CenterPoint().y, true);
	DrawPacman(&memDC, pacmanPosition, mouthAngle);
	
	
	memDC.SetWorldTransform(&oldWT);

	DrawGhost(&memDC, ghostPosition);

	if (gameOver) {
		CFont font;

		font.CreateFontW(
			72, 0, 0, 0,
			FW_BOLD,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			L"Arial"
		);

		CFont* oldFont = memDC.SelectObject(&font);

		int oldBkMode = memDC.SetBkMode(TRANSPARENT);
		auto oldTextColor = memDC.SetTextColor(RGB(255, 255, 255));

		memDC.DrawText(
			L"The End",
			cr,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE
		);

		memDC.SelectObject(oldFont);
		memDC.SetBkMode(oldBkMode);
		memDC.SetTextColor(oldTextColor);
	}


	/**
	*
	*/
	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &memDC, 0, 0, SRCCOPY);


}

void CPacmanView::Translate(CDC* pDC, int dX, int dY, bool rightMultiply) {
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPacmanView::Rotate(CDC* pDC, float angle, bool rightMultiply) {
	float angleRad = TO_RAD(angle);
	XFORM m = {
		cos(angleRad), -sin(angleRad),
		sin(angleRad), cos(angleRad),
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPacmanView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply) {
	XFORM m = {
		sX, 0,
		0, sY,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPacmanView::DrawPacman(CDC* pDC, CRect rect, float angle) {

	int dX = cos(TO_RAD(angle)) * rect.Width() / 2;
	int dY = sin(TO_RAD(angle)) * rect.Height() / 2;

	int eyeX = rect.CenterPoint().x + dX / 2;
	int eyeY = rect.CenterPoint().y - max(dY, 15);

	CPoint start(rect.CenterPoint());
	start.Offset(dX, -dY);
	CPoint end(rect.CenterPoint());
	end.Offset(dX, dY);

	CPen blackPen(PS_GEOMETRIC, 2, RGB(0, 0, 0));
	auto oldPen = pDC->SelectObject(&blackPen);

	CBrush yellowBrush(RGB(200, 255, 50));
	auto oldBrush = pDC->SelectObject(&yellowBrush);
	
	pDC->Pie(rect, start, end);

	CBrush blackBrush(RGB(0, 0, 0));
	pDC->SelectObject(&blackBrush);

	pDC->Ellipse(eyeX - 5, eyeY - 5, eyeX + 5, eyeY + 5);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CPacmanView::DrawGhost(CDC* pDC, CRect rect) {

	CPen pen(PS_GEOMETRIC, 2, RGB(0, 0, 0));
	auto oldPen = pDC->SelectObject(&pen);

	CBrush brush(RGB(255, 0, 0));
	auto oldBrush = pDC->SelectObject(&brush);

	int r = rect.CenterPoint().x - rect.left;

	CPoint arcStart(rect.CenterPoint());
	arcStart.Offset(-r, 0);
	CPoint arcEnd(rect.CenterPoint());
	arcEnd.Offset(r, 0);
	
	pDC->BeginPath();

	pDC->MoveTo(arcStart);

	int arcDirection = pDC->SetArcDirection(AD_CLOCKWISE);
	pDC->ArcTo(rect, arcStart, arcEnd);


	CPoint lowerLeft(rect.left, rect.bottom);
	CPoint lowerRight(rect.right, rect.bottom);

	pDC->LineTo(lowerRight);

	CRect mini(lowerRight.x - r/6, lowerRight.y - r/3, lowerRight.x + r/6, lowerRight.y);


	CPoint p;
	p.SetPoint(lowerRight.x, lowerRight.y);
	CPoint p2;
	p2.SetPoint(lowerRight.x - r/6, lowerRight.y - r/6);

	pDC->SetArcDirection(AD_CLOCKWISE);
	pDC->ArcTo(mini, p, p2);

	for (int i = 0; i < 5; i++) {
		mini.OffsetRect(-r / 3, 0);
		p = p2;
		p2.Offset(-r / 3, 0);
		pDC->SetArcDirection(i % 2 == 0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE);
		pDC->ArcTo(mini, p, p2);
	}
	
	mini.OffsetRect(-r / 3, 0);
	p.SetPoint(p2.x - r / 6, p2.y + r / 6);

	pDC->SetArcDirection(AD_CLOCKWISE);
	pDC->ArcTo(mini, p2, p);
	
	pDC->LineTo(arcStart);

	pDC->EndPath();
	pDC->CloseFigure();

	pDC->StrokeAndFillPath();

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);

	DrawGhostEye(pDC, rect.CenterPoint(), r/4, r/8);
	DrawGhostEye(pDC, CPoint(rect.CenterPoint().x + r*2/3, rect.CenterPoint().y), r / 4, r / 8);
}

void CPacmanView::DrawGhostEye(CDC* pDC, CPoint center, int eyeRadius, int irisRadius) {

	CBrush whiteBrush(RGB(255, 255, 255));
	auto oldBrush = pDC->SelectObject(&whiteBrush);
	
	pDC->Ellipse(center.x - eyeRadius - irisRadius / 3, center.y - eyeRadius,
				 center.x + eyeRadius - irisRadius / 3, center.y + eyeRadius);

	CBrush blueBrush(RGB(0, 0, 255));
	pDC->SelectObject(&blueBrush);

	pDC->Ellipse(center.x - irisRadius, center.y - irisRadius,
				 center.x + irisRadius, center.y + irisRadius);

	pDC->SelectObject(oldBrush);
}

void CPacmanView::DetectCollision() {
	
	int pacmanRadius = min(pacmanPosition.Width(), pacmanPosition.Height()) / 2;
	int ghostRadius = min(ghostPosition.Width(), ghostPosition.Height()) / 2;
	int collisionRadius = pacmanRadius + ghostRadius;

	CRgn collisionRegion;
	collisionRegion.CreateEllipticRgn(ghostPosition.CenterPoint().x - collisionRadius, ghostPosition.CenterPoint().y - collisionRadius, 
									  ghostPosition.CenterPoint().x + collisionRadius, ghostPosition.CenterPoint().y + collisionRadius);

	if (collisionRegion.PtInRegion(pacmanPosition.CenterPoint()))
		gameOver = true;
}

// CPacmanView printing

BOOL CPacmanView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPacmanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPacmanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CPacmanView diagnostics

#ifdef _DEBUG
void CPacmanView::AssertValid() const
{
	CView::AssertValid();
}

void CPacmanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPacmanDoc* CPacmanView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPacmanDoc)));
	return (CPacmanDoc*)m_pDocument;
}
#endif //_DEBUG

UINT CPacmanView::OnGetDlgCode() {
	return DLGC_WANTARROWS;
}
void CPacmanView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if (!gameOver) {
		switch (nChar) {
			case VK_LEFT:
			mouthAngle = mouthAngle == 0 ? 30 : 0;
			pacmanPosition.OffsetRect(-10, 0);
			pacmanAngle = 0;
			mirrorPacman = true;
			break;
			case VK_RIGHT:
			mouthAngle = mouthAngle == 0 ? 30 : 0;
			pacmanPosition.OffsetRect(10, 0);
			pacmanAngle = 0;
			mirrorPacman = false;
			break;
			case VK_UP:
			mouthAngle = mouthAngle == 0 ? 30 : 0;
			pacmanPosition.OffsetRect(0, -10);
			pacmanAngle = 90;
			break;
			case VK_DOWN:
			mouthAngle = mouthAngle == 0 ? 30 : 0;
			pacmanPosition.OffsetRect(0, 10);
			pacmanAngle = -90;
			break;
		}
	}

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
