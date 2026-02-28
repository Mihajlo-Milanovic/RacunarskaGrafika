#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#include <string>
//#pragma comment(lib, "GL\\glut32.lib")

#define RAD(angle) (angle * 3.141592 / 180)

CGLRenderer::CGLRenderer(void)
{
	alpha = 0.0f;
	beta = 0.0f;
	moonRot = 0;
	distance = 10;
	lightOn = false;

	rotating = false;
	camDistance = 0;
	camPitch = 0;
	camYaw = 0;
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(1.0f, 1.0f, 1.0f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	for (int i = 0; i < 6; i++) {
		
		earth[i] = LoadTexture(("../RES/TSC" + std::to_string(i) + ".jpg").c_str());
		moon[i] = LoadTexture(("../RES/M" + std::to_string(i) + ".jpg").c_str());
		space[i] = LoadTexture(("../RES/S" + std::to_string(i) + ".jpg").c_str());
	}

	glEnable(GL_TEXTURE_2D);
	//earth[0] = LoadTexture("..\\RES\\TSC0.jpg");
	//earth[1] = LoadTexture("..\\RES\\TSC1.jpg");
	//earth[2] = LoadTexture("..\\RES\\TSC2.jpg");
	//earth[3] = LoadTexture("..\\RES\\TSC3.jpg");
	//earth[4] = LoadTexture("..\\RES\\TSC4.jpg");
	//earth[5] = LoadTexture("..\\RES\\TSC5.jpg");

	//moon[0] = LoadTexture("..\\RES\\M0.jpg");
	//moon[1] = LoadTexture("..\\RES\\M1.jpg");
	//moon[2] = LoadTexture("..\\RES\\M2.jpg");
	//moon[3] = LoadTexture("..\\RES\\M3.jpg");
	//moon[4] = LoadTexture("..\\RES\\M4.jpg");
	//moon[5] = LoadTexture("..\\RES\\M5.jpg");


	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	SetCameraPosition();

	glPushMatrix();

	glTranslatef(0, 0, distance);
	glRotated(beta, 1, 0, 0);
	glRotated(alpha, 0, 1, 0);
	DrawSpace(1000, 20);
	glPopMatrix();

	if (lightOn) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	float ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	float diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	float specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	float lightPos[] = { 0.0, 0.0, 1.0 };

	glEnable(GL_DEPTH_TEST);

	glRotated(beta, 1, 0, 0);
	glRotated(alpha, 0, 1, 0);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	DrawEarth(3, 20);

	glTranslatef(-50, 0, 0);
	glRotatef(moonRot, 0, 1, 0);
	DrawMoon(1, 20);

	glFlush();
	SwapBuffers(pDC->m_hDC);
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(40.0, (GLdouble)w / (GLdouble)h, 0.1, 2000.0);

	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(6, earth);
	glDeleteTextures(6, moon);
	glDeleteTextures(6, space);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(const char* fileName) {
	
	UINT textureID = 0;
	DImage img;
	img.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return textureID;
}

void CGLRenderer::DrawPatch(double R, int n) {

	double delta = 2.0 / (double) n;
	double y = 1.0;

	for (int i = 0; i < n; i++) {
		glBegin(GL_TRIANGLE_STRIP);

		double x = -1.0;
		for (int j = 0; j < n + 1; j++) {

			double phi, theta;
			InverseTSC(x, y, phi, theta);

			double xdn, ydn, zdn;
			xdn = cos(theta) * sin(phi);
			ydn = sin(theta);
			zdn = cos(theta) * cos(phi);

			glNormal3f(xdn, ydn, zdn);
			glTexCoord2f((x + 1) / 2, (-y + 1) / 2);
			glVertex3f(R * xdn, R * ydn, R * zdn);

			InverseTSC(x, y - delta, phi, theta);

			xdn = cos(theta) * sin(phi);
			ydn = sin(theta);
			zdn = cos(theta) * cos(phi);

			glNormal3f(xdn, ydn, zdn);
			glTexCoord2f((x + 1) / 2, (-y + delta + 1) / 2);
			glVertex3f(R * xdn, R * ydn, R * zdn);

			x += delta;
		}
		glEnd();
		y -= delta;
	}
}

void CGLRenderer::DrawEarth(double R, int tes) {
	
	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, earth[i]);
		DrawPatch(R, tes);
		glRotated(90, 0, 1, 0);
	}
	
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, earth[4]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, earth[5]);
	DrawPatch(R, tes);
	glPopMatrix();
}

void CGLRenderer::DrawMoon(double R, int tes) {
	
	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, moon[i]);
		DrawPatch(R, tes);
		glRotated(90, 0, 1, 0);
	}
	
	glPopMatrix();
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, moon[4]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, moon[5]);
	DrawPatch(R, tes);
	glPopMatrix();
}

void CGLRenderer::DrawSpace(double R, int tes) {

	glDisable(GL_CULL_FACE);

	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, space[i]);
		DrawPatch(R, tes);
		glRotated(90, 0, 1, 0);
	}

	glPopMatrix();
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, space[4]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, space[5]);
	DrawPatch(R, tes);
	glPopMatrix();

	glEnable(GL_CULL_FACE);
}

void CGLRenderer::InverseTSC(double x, double y, double& phi, double& theta) {
	phi = atan(x);
	theta = atan(y * cos(phi));
}

void CGLRenderer::SetCameraPosition() {

	float radYaw = RAD(camYaw);
	float radPitch = RAD(camPitch);

	float x = camDistance * cos(radPitch) * sin(radYaw);
	float y = camDistance * sin(radPitch);
	float z = camDistance * cos(radPitch) * cos(radYaw);

	gluLookAt(x, y, z+distance,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f, 0.0f);
}
