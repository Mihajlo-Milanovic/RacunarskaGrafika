
// PuzzleView.cpp : implementation of the CPuzzleView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Puzzle.h"
#endif

#include "PuzzleDoc.h"
#include "PuzzleView.h"
#include "DImage.h"
#include <format>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.14159
#define TO_RAD(a) (a/180*PI)
#define imgCenterX 127
#define imgCenterY 135

// CPuzzleView

IMPLEMENT_DYNCREATE(CPuzzleView, CView)

BEGIN_MESSAGE_MAP(CPuzzleView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
//	ON_WM_RBUTTONDBLCLK()
ON_WM_RBUTTONUP()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CPuzzleView construction/destruction

CPuzzleView::CPuzzleView() noexcept
{
	// TODO: add construction code here

	
	pieces = new DImage[9];

	pieces[0].Load(CString("puzzle_pieces/0.bmp"));
	pieces[1].Load(CString("puzzle_pieces/1.bmp"));
	pieces[2].Load(CString("puzzle_pieces/2.bmp"));

	pieces[3].Load(CString("puzzle_pieces/3.bmp"));
	pieces[4].Load(CString("puzzle_pieces/4.bmp"));
	pieces[5].Load(CString("puzzle_pieces/5.bmp"));

	pieces[6].Load(CString("puzzle_pieces/6.bmp"));
	pieces[7].Load(CString("puzzle_pieces/7.bmp"));
	pieces[8].Load(CString("puzzle_pieces/8.bmp"));


	imgW = pieces[0].Width();
	imgH = pieces[0].Height();

	positions = new CRect[9];
	for (int i = 0; i < 9; i++) {
		positions[i] = { 0, 0, imgCenterX, imgCenterY };
	}

	rotations = new float[9];
	for (int i = 0; i < 9; i++) {
		rotations[i] = 0.0;
	}

	
	firstDraw = true;
	selectedPiece = -1;
	holdingAPiece = false;
}

CPuzzleView::~CPuzzleView()
{
	delete[] pieces;
	delete[] positions;
	delete[] rotations;
}

BOOL CPuzzleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CPuzzleView drawing

void CPuzzleView::OnDraw(CDC* pDC) {

	CPuzzleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	//CView::OnInitialUpdate();

	CFrameWnd* pFrame = GetParentFrame();
	if (!pFrame) return;

	// Remove title bar, borders
	/*pFrame->ModifyStyle(WS_CAPTION | WS_THICKFRAME, 0);
	pFrame->ModifyStyleEx(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
						  WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);*/

	// Maximize
	pFrame->ShowWindow(SW_MAXIMIZE);



	

	CRect cr;
	GetClientRect(&cr);
	
	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);

	CBitmap backBmp;
	backBmp.CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());
	CBitmap* oldBmp = memDC->SelectObject(&backBmp);
	memDC->FillSolidRect(cr, RGB(255, 255, 255));

	auto oldGM = memDC->SetGraphicsMode(GM_ADVANCED);


	DrawPuzzle(memDC);
		
	CString txt;
	if(selectedPiece > -1)
		txt.Format(L"Selected piece: %d   Coords: [%d], [%d]", 
					selectedPiece,
					positions[selectedPiece].left,
					positions[selectedPiece].top
		);
	else
		txt.Format(L"Selected piece: NONE   Coords: [/], [/]");

	memDC->TextOutW(0, cr.Height()-30, txt);

	


	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), memDC, 0, 0, SRCCOPY);

	memDC->SetGraphicsMode(oldGM);
}

void CPuzzleView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false){
	XFORM m = {
		1, 0,
		0, 1,
		dX, dY
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPuzzleView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false) {
	XFORM m = {
		sX, 0,
		0, sY,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPuzzleView::Rotate(CDC* pDC, float angle, bool rightMultiply = false) {
	float rad = TO_RAD(angle);
	float cosA = cos(rad);
	float sinA = sin(rad);
	XFORM m = {
		cosA, sinA,
		-sinA, cosA,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPuzzleView::Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply = false) {
	XFORM m = {
		mx ? -1 : 1, 0,
		0, my ? -1 : 1,
		0, 0
	};
	pDC->ModifyWorldTransform(&m, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CPuzzleView::DrawTransparent(CDC* pDC, DImage &img, LONG left = 0, LONG top = 0) {

	CBitmap bmpMask;

	bmpMask.CreateBitmap(img.Width(), img.Height(), 1, 1, NULL);

	CDC* srcDC = new CDC();
	srcDC->CreateCompatibleDC(pDC);
	CDC* destDC = new CDC();
	destDC->CreateCompatibleDC(pDC);

	CBitmap *oldSrcBmp = srcDC->SelectObject(img.GetBitmap());
	CBitmap *oldDestBmp = destDC->SelectObject(&bmpMask);

	COLORREF clrTopLeft = srcDC->GetPixel(1, 1);
	COLORREF oldBkColor = srcDC->SetBkColor(clrTopLeft);
	destDC->BitBlt(0, 0, img.Width(), img.Height(), srcDC, 0, 0, SRCCOPY);

	COLORREF oldSrcTextColor = srcDC->SetTextColor(RGB(255, 255, 255));
	srcDC->SetBkColor(RGB(0, 0, 0));
	srcDC->BitBlt(0, 0, img.Width(), img.Height(), destDC, 0, 0, SRCAND);


	srcDC->SetTextColor(oldSrcTextColor);
	srcDC->SetBkColor(oldBkColor);

	srcDC->SelectObject(oldSrcBmp);

	pDC->BitBlt(left, top, img.Width(), img.Height(), destDC, 0, 0, SRCAND);

	destDC->SelectObject(img.GetBitmap());
	pDC->BitBlt(left, top, img.Width(), img.Height(), destDC, 0, 0, SRCPAINT);
	destDC->SelectObject(oldDestBmp);

	srcDC->DeleteDC();
	delete srcDC;
	
	destDC->DeleteDC();
	delete destDC;
}

void CPuzzleView::DrawPuzzle(CDC* pDC) {

	//CBrush transparentBrush(TRANSPARENT);
	pDC->SelectStockObject(NULL_BRUSH);

	CRect cr;
	GetClientRect(&cr);

	XFORM oldWT;
	pDC->GetWorldTransform(&oldWT);

	for (int i = 8; i > -1; i--) {

		if (selectedPiece == i)
			pDC->Rectangle(
				positions[selectedPiece].left + 55, positions[selectedPiece].top + 70,
				positions[selectedPiece].right - 55, positions[selectedPiece].bottom - 65
			);

		if (firstDraw) {
			//Translate(pDC, positions[i].left + imgCenterX + i * 200, positions[i].top + imgCenterY);
			positions[i].CopyRect(CRect{
				-imgCenterX + i * cr.Width() / 9 + 100, -imgCenterY + 100 ,
				imgCenterX + i * cr.Width() / 9 + 100, imgCenterY + 100
			});
			DrawTransparent(pDC, pieces[i], -imgCenterX + i * cr.Width() / 8, -imgCenterY);
			
		}
		else {
			Rotate(pDC, rotations[i]);
			Translate(pDC, positions[i].left + imgCenterX, positions[i].top + imgCenterY, true);
			DrawTransparent(pDC, pieces[i], -imgCenterX, -imgCenterY);
		}

		pDC->SetWorldTransform(&oldWT);
	}
	firstDraw = false;
}

// CPuzzleView printing

BOOL CPuzzleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPuzzleView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPuzzleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}




// CPuzzleView diagnostics

#ifdef _DEBUG
void CPuzzleView::AssertValid() const
{
	CView::AssertValid();
}

void CPuzzleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPuzzleDoc* CPuzzleView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPuzzleDoc)));
	return (CPuzzleDoc*)m_pDocument;
}
#endif //_DEBUG


// CPuzzleView message handlers

void CPuzzleView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar) {
		case 'a':
			if (selectedPiece > -1) {
				rotations[selectedPiece] -= 1.0;
				Invalidate(FALSE);
			}
		break;
		case 'A':
			if (selectedPiece > -1) {
				rotations[selectedPiece] -= 90.0;
				Invalidate(FALSE);
			}
		break;
		case 'd':
			if (selectedPiece > -1) {
				rotations[selectedPiece] += 1.0;
				Invalidate(FALSE);
			}
		break;
		case 'D':
			if (selectedPiece > -1) {
				rotations[selectedPiece] += 90.0;
				Invalidate(FALSE);
			}
		break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			selectedPiece = nChar - '0';
			Invalidate(FALSE);
		break;
		case ' ':
			selectedPiece = selectedPiece < 8 ? selectedPiece + 1 : 0;
			Invalidate(FALSE);
		break;
		
	}

	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CPuzzleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	for (int i = 0; i < 9; i++) {
		
		if (CRect(positions[i].left + 55, positions[i].top + 70,
				  positions[i].right - 55, positions[i].bottom - 65
				).PtInRect(point)
		) {
			selectedPiece = i;
			holdingAPiece = true;
			//lastMousePosition = point;
			Invalidate(FALSE);
			break;
		}
	}

	CView::OnLButtonDown(nFlags, point);
}

void CPuzzleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (selectedPiece > -1) {
		
		positions[selectedPiece] = {
			point.x - imgCenterX, point.y - imgCenterY,
			point.x + imgCenterX, point.y + imgCenterY
		};
		selectedPiece = -1;
		holdingAPiece = false;
		Invalidate(FALSE);
	}

	CView::OnLButtonUp(nFlags, point);
}

void CPuzzleView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	
	CView::OnRButtonUp(nFlags, point);
}

void CPuzzleView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (holdingAPiece && selectedPiece != -1) {

		positions[selectedPiece] = {
			point.x - imgCenterX, point.y - imgCenterY,
			point.x + imgCenterX, point.y + imgCenterY
		};

		//lastMousePosition = point;
		Invalidate(FALSE);
	}

	CView::OnMouseMove(nFlags, point);
}
