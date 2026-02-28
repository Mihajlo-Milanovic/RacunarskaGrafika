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

	void SetCamera();
	void DrawAxes(float len);

	UINT CGLRenderer::LoadTexture(char* fileName);
	void CGLRenderer::DrawTriangle(float d1, float d2, float rep); 
	void CGLRenderer::DrawShip();
	void DrawShipHalf(double d1, double d2);
	void CGLRenderer::DrawSpaceCube(double a);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	UINT shipTexId;
	UINT topTexId;
	UINT bottomTexId;
	UINT leftTexId;
	UINT rightTexId;
	UINT frontTexId;
	UINT backTexId;

	float camX;
	float camY;
	float camZ;

	double d1;
	double d2;
	
public:

	bool lightOn;

	//mouse control of the camera for debugging purposes
	float camPitch;
	float camYaw;
	float camDistance;
	bool camRotating;
	CPoint lastMousePos;
};
