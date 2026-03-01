#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

#define PI 3.1415926535897932384626433832795
#define RAD(x) ((x)*PI/180.0)

CGLRenderer::CGLRenderer(void)
{
	camDistance = 30;
	camYaw = 30.0;
	camPitch = 45;
	isRotating = false;

	camX = 0;
	camY = 10;
	camZ = 0;
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	spiderTexture = LoadTexture("../../spider.png");
	frontTexture = LoadTexture("../../front.jpg");
	backTexture = LoadTexture("../../back.jpg");
	topTexture = LoadTexture("../../top.jpg");
	bottomTexture = LoadTexture("../../bot.jpg");
	leftTexture = LoadTexture("../../left.jpg");
	rightTexture = LoadTexture("../../right.jpg");
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	SetCamera();	

	gluLookAt(camX, camY, camZ,
			  0, 0, 0,
			  0, 1, 0);


	DrawEnvCube(100);
	DrawAxes();
	DrawSpider();

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

	gluPerspective(50.0, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);

	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &spiderTexture);

	glDeleteTextures(1, &frontTexture);
	glDeleteTextures(1, &backTexture);
	glDeleteTextures(1, &topTexture);
	glDeleteTextures(1, &bottomTexture);
	glDeleteTextures(1, &leftTexture);
	glDeleteTextures(1, &rightTexture);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes() {

	glLineWidth(2.0);

	glBegin(GL_LINES);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);

	glEnd();
}

void CGLRenderer::SetCamera() {
	
	double radYaw = RAD(camYaw);
	double radPitch = RAD(camPitch);

	camY = camDistance * sin(radPitch);
	//if()

	camX = camDistance * cos(radPitch) * cos(radYaw);
	camZ = camDistance * cos(radPitch) * sin(radYaw);
}

UINT CGLRenderer::LoadTexture(char* fileName) {

	UINT texId;
	DImage img;
	img.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texId;
}

void CGLRenderer::DrawEnvCube(double a) {

	double halfA = a / 2;

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	
	//glTranslated(camX, camY, camZ);

	glBindTexture(GL_TEXTURE_2D, backTexture);
	{
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0); glVertex3d(halfA, halfA, halfA);
		glTexCoord2f(0, 1); glVertex3d(halfA, -halfA, halfA);
		glTexCoord2f(1, 0); glVertex3d(-halfA, halfA, halfA);
		glTexCoord2f(1, 1); glVertex3d(-halfA, -halfA, halfA);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, frontTexture);
	{
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0); glVertex3d(-halfA, halfA, -halfA);
		glTexCoord2f(0, 1); glVertex3d(-halfA, -halfA, -halfA);
		glTexCoord2f(1, 0); glVertex3d(halfA, halfA, -halfA);
		glTexCoord2f(1, 1); glVertex3d(halfA, -halfA, -halfA);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, leftTexture);
	{
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0); glVertex3d(-halfA, halfA, halfA);
		glTexCoord2f(0, 1); glVertex3d(-halfA, -halfA, halfA);
		glTexCoord2f(1, 0); glVertex3d(-halfA, halfA, -halfA);
		glTexCoord2f(1, 1); glVertex3d(-halfA, -halfA, -halfA);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, rightTexture);
	{
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0); glVertex3d(halfA, halfA, -halfA);
		glTexCoord2f(0, 1); glVertex3d(halfA, -halfA, -halfA);
		glTexCoord2f(1, 0); glVertex3d(halfA, halfA, halfA);
		glTexCoord2f(1, 1); glVertex3d(halfA, -halfA, halfA);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, topTexture);
	{
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0); glVertex3d(-halfA, halfA, halfA);
		glTexCoord2f(0, 1); glVertex3d(halfA, halfA, halfA);
		glTexCoord2f(1, 0); glVertex3d(-halfA, halfA, -halfA);
		glTexCoord2f(1, 1); glVertex3d(halfA, halfA, -halfA);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, bottomTexture);
	{
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0); glVertex3d(halfA, -halfA, -halfA);
		glTexCoord2f(0, 1); glVertex3d(-halfA, -halfA, -halfA);
		glTexCoord2f(1, 0); glVertex3d(halfA, -halfA, halfA);
		glTexCoord2f(1, 1); glVertex3d(-halfA, -halfA, halfA);
		glEnd();
	}

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR) {

	double step = PI / (double)nSeg;

	glPointSize(3);
	glColor4f(1, 1, 1, 1);
	
	for (double lat = -PI/2; lat < PI/2; lat += step) {

		glBegin(GL_QUAD_STRIP);
		for (double lng = 0; lng <= 2 * PI; lng += step) {


			double xn = cos(lat) * cos(lng);
			double yn = sin(lat);
			double zn = cos(lat) * sin(lng);

			glTexCoord2d(texU + xn * texR, texV + zn * texR);
			glNormal3d(xn, yn, zn);
			glVertex3d(xn * r, yn * r, zn * r);

			xn = cos(lat + step) * cos(lng);
			yn = sin(lat + step);
			zn = cos(lat + step) * sin(lng);

			glTexCoord2d(texU + xn * texR, texV + zn * texR);
			glNormal3d(xn, yn, zn);
			glVertex3d(xn * r, yn * r, zn * r);
		}
		glEnd();
	}
	
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR) {

	double step = (2 * PI) / (double)nSeg;

	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(texU, texV);
	glNormal3d(0, 1, 0);
	glVertex3d(0, h, 0);

	for (double i = 0; i <= 2 * PI; i += step) {
		
		double xn = cos(i);
		double zn = sin(i);

		glTexCoord2f(texU + xn * texR, texV + zn * texR);
		glNormal3d(xn, 1, zn);
		glVertex3d(r * xn, 0, r * zn);
	}

	glEnd();

	//base
	glBegin(GL_TRIANGLE_FAN);

	glVertex3d(0, 0, 0);

	for (double i = 0; i <= 2 * PI; i += step) {

		double xn = cos(i);
		double zn = sin(i);

		glVertex3d(r * xn, 0, r * zn);
	}

	glEnd();
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg) {

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spiderTexture);

	DrawSphere(r, 2 * nSeg, 0.25, 0.25, 0.25);
	DrawCone(r, h, nSeg, 0.75, 0.75, 0.25);

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawLeg() {

	glPushMatrix();

	DrawLegSegment(1, 10, 5);
	glTranslated(0, 10, 0);
	glRotated(85, 1, 0, 0);
	DrawLegSegment(1, 15, 5);

	glPopMatrix();
}

void CGLRenderer::DrawSpiderBody() {

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spiderTexture);
	glPushMatrix();

	glScaled(1, 0.5, 1);
	DrawSphere(3, 10, 0.25, 0.25, 0.25); //middle part

	glTranslated(-5, 0, 0);
	DrawSphere(2, 10, 0.75, 0.25, 0.25); //head

	glTranslated(13, 0, 0);
	glScaled(1, 1.6, 1);
	DrawSphere(5, 10, 0.25, 0.25, 0.25);

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSpider() {

	glPushMatrix();

	DrawSpiderBody();


	//left side legs
	glRotated(45, 1, 0, 0);
	DrawLeg();

	glPopMatrix();
	glPushMatrix();

	glRotated(30, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	DrawLeg();

	glPopMatrix();
	glPushMatrix();

	glRotated(-30, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	DrawLeg();

	glPopMatrix();

	//right side legs
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	DrawLeg();

	glPopMatrix();
	glPushMatrix();

	glRotated(210, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	DrawLeg();

	glPopMatrix();
	glPushMatrix();

	glRotated(150, 0, 1, 0);
	glRotated(45, 1, 0, 0);
	DrawLeg();

	glPopMatrix();


}