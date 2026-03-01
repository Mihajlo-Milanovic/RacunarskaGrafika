#pragma once

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	void CGLRenderer::DrawAxes();
	void SetCamera();
	UINT CGLRenderer::LoadTexture(char* fileName);
	void CGLRenderer::DrawEnvCube(double a);
	void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR);
	void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR);
	void CGLRenderer::DrawLegSegment(double r, double h, int nSeg);
	void CGLRenderer::DrawLeg();
	void CGLRenderer::DrawSpiderBody();
	void CGLRenderer::DrawSpider();

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	UINT spiderTexture;
	UINT frontTexture;
	UINT leftTexture;
	UINT rightTexture;
	UINT backTexture;
	UINT topTexture;
	UINT bottomTexture;

public:
	double camX;
	double camY;
	double camZ;

	float camDistance;
	float camYaw;
	float camPitch;
	bool isRotating;
	CPoint lastMousePos;
};
