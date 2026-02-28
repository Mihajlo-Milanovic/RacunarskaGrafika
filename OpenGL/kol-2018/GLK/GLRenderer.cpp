#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

#define RAD(x) ((x)*3.1415926535897932384626433832795/180.0)

CGLRenderer::CGLRenderer(void)
{
	camDistance = 8.0f;
	camPitch = 45.0f;
	camYaw = 45.0f;
	camRotating = false;
	lightOn = true;

	d1 = 5.8;
	d2 = 2.15;
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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);
	
	shipTexId = LoadTexture("../ShipT1.png");
	topTexId = LoadTexture("../top.jpg");
	bottomTexId = LoadTexture("../bot.jpg");
	leftTexId = LoadTexture("../left.jpg");
	rightTexId = LoadTexture("../right.jpg");
	frontTexId = LoadTexture("../front.jpg");
	backTexId = LoadTexture("../back.jpg");
	
	glEnable(GL_TEXTURE_2D);

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

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	DrawSpaceCube(20.0);

	glEnable(GL_DEPTH_TEST);

	//glTranslatef(0, 0, camDistance);
	//glRotated(camPitch, 1.0, 0.0, 0.0);
	//glRotated(camYaw, 0.0, 1.0, 0.0);

	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	DrawAxes(10.0f);

	if (lightOn) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	float hypotenuse = sqrt(d1 * d1 + d2 * d2);
	glRotatef(90, 1, 0, 0);
	glTranslatef(-hypotenuse / 2, 0, 0);
	DrawShip();
	

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

	gluPerspective(50.0f, (double)w / (double)h, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &shipTexId);
	glDeleteTextures(1, &topTexId);
	glDeleteTextures(1, &bottomTexId);
	glDeleteTextures(1, &leftTexId);
	glDeleteTextures(1, &rightTexId);
	glDeleteTextures(1, &frontTexId);
	glDeleteTextures(1, &backTexId);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::SetCamera()
{
	float radYaw = RAD(camYaw);
	float radPitch = RAD(camPitch);

	camX = camDistance * cos(radPitch) * sin(radYaw);
	camY = camDistance * sin(radPitch);
	camZ = camDistance * cos(radPitch) * cos(radYaw);


	gluLookAt(camX, camY, camZ,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
}

void CGLRenderer::DrawAxes(float len) {
	glBegin(GL_LINES);

	//X
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(len, 0.0, 0.0);
	//Y
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, len, 0.0);
	//Z
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, len);

	glEnd();
}

UINT CGLRenderer::LoadTexture(char* fileName) {

	UINT texID;
	DImage img;
	img.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::DrawTriangle(float d1, float d2, float rep) {

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, shipTexId);


	double hypotenuse = sqrt(d1 * d1 + d2 * d2);

	double r = rep / hypotenuse;

	double alpha = asin(d1 / hypotenuse);
	double dx = sin(alpha) * (d2 / hypotenuse);
	double dy = cos(alpha) * (d2 / hypotenuse);

	glBegin(GL_TRIANGLES);

	glColor3f(1.0, 1.0, 1.0);
	glNormal3d(0, 0, 1);

	//*--*--*--*--*--*--*--*--*--*--*--*--*--*--*

	//using right half of the texture
	// setting texture offset to -0.5 for X
	glTexCoord2f(0.5*rep, 0);
	glVertex2f(0, 0);

	glTexCoord2f((0.5 + dx)*rep, (1 - dy) * rep);
	glVertex2f(d1, 0);

	glTexCoord2f(0.5*rep, rep);
	glVertex2f(d1, d2);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawShip() {

	glPushMatrix();
	DrawShipHalf(d1, d2);
	glPopMatrix();

	glPushMatrix();
	glScalef(1.0, 1.0, -1.0);
	DrawShipHalf(d1, d2);
	glPopMatrix();
}

void CGLRenderer::DrawShipHalf(double d1, double d2) {
	
	glRotatef(13, 1, 0, 0);
	glRotated(4.75, 0, 1, 0);
	DrawTriangle(d1, d2, 3.0f);

	glRotatef(154, 1, 0, 0); // 180-26
	DrawTriangle(d1, d2, 3.0f);
}

void CGLRenderer::DrawSpaceCube(double a) {

	//front face vertices	//back face vertices
	//						//	
	//	3---2				//	7---6
	//	|   |				//	|   |
	//	0---1				//	4---5
	
	float half = a / 2.0f;
	glPushMatrix();

	glTranslatef(camX, camY, camZ);
	//glRotatef(camYaw, 0, 1, 0);
	//glRotatef(-camPitch, 1, 0, 0);

	glColor3f(1.0, 1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, frontTexId);
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-half, -half, half);
		glTexCoord2f(0, 1); glVertex3f(half, -half, half);
		glTexCoord2f(1, 1); glVertex3f(half, half, half);
		glTexCoord2f(1, 0); glVertex3f(-half, half, half);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, backTexId);
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-half, -half, -half);
		glTexCoord2f(1, 0); glVertex3f(half, -half, -half);
		glTexCoord2f(1, 1); glVertex3f(half, half, -half);
		glTexCoord2f(0, 1); glVertex3f(-half, half, -half);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, topTexId);
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex3f(-half, half, half);
		glTexCoord2f(0, 0); glVertex3f(half, half, half);
		glTexCoord2f(1, 0); glVertex3f(half, half, -half);
		glTexCoord2f(1, 1); glVertex3f(-half, half, -half);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, bottomTexId);
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-half, -half, -half);
		glTexCoord2f(1, 0); glVertex3f(half, -half, -half);
		glTexCoord2f(1, 1); glVertex3f(half, -half, half);
		glTexCoord2f(0, 1); glVertex3f(-half, -half, half);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, leftTexId);
	{
		glBegin(GL_QUADS);
		glTexCoord2f(1, 1); glVertex3f(-half, -half, half);
		glTexCoord2f(0, 1); glVertex3f(-half, -half, -half);
		glTexCoord2f(0, 0); glVertex3f(-half, half, -half);
		glTexCoord2f(1, 0); glVertex3f(-half, half, half);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, rightTexId);
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(half, -half, -half);
		glTexCoord2f(1, 0); glVertex3f(half, -half, half);
		glTexCoord2f(1, 1); glVertex3f(half, half, half);
		glTexCoord2f(0, 1); glVertex3f(half, half, -half);
		glEnd();
	}

	glPopMatrix();
}