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

	UINT LoadTexture(const char* fileName);

	void DrawPatch(double R, int n);
	void DrawEarth(double R, int tes);
	void DrawMoon(double R, int tes);
	void DrawSpace(double R, int tes);

	void InverseTSC(double x, double y, double& phi, double& theta);
	void SetCameraPosition();

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	UINT earth[6];
	UINT moon[6];
	UINT space[6];

	float eX, eY, eZ;

public:
	float alpha, beta;
	float moonRot;
	float distance;
	bool lightOn;

	float camDistance;
	float camPitch, camYaw;
	CPoint lastMousePos;
	bool rotating;
};
