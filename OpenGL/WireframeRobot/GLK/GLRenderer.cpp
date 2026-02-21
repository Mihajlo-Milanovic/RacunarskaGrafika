#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

#define PI 3.14159265358979323846f
#define DEG2RAD(x) ((x) * PI / 180.0f)
#define RAD2DEG(x) ((x) * 180.0f / PI)

CGLRenderer::CGLRenderer(void) {
	m_hrc = NULL;

	camYaw = 45.0f;
	camPitch = 30.0f;
	camDistance = 10.0f;
	isRotating = false;

	leftUpperArmAngle = 0.0;
	rightUpperArmAngle = 0.0;
	
	leftForearmAngle = 0.0;
	rightForearmAngle = 0.0;

	bodyAngle = 0.0;
	headAngle = 0.0;
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	SetCameraPosition();
	
	DrawGrid(5.0, 20);
	DrawAxes(10.0);
	DrawRobot();
	
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

	gluPerspective(45.0f, (double)w / (double)h, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawGrid(double dSize, int nSteps) {

	if (nSteps < 1)
		return;

	const float step = 2 * dSize / nSteps;
	const int nLines = nSteps + 1;
	const int nPoints = nLines * 12;

	float* points = new float[nPoints];

	int index = 0;
	for (int i = 0; i < nLines; i++) {

		float v = -dSize + i * step;

		//X 1
		points[index++] = v;
		points[nPoints - index] = v;
		//Y 1
		points[index++] = 0.0f;
		points[nPoints - index] = 0.0f;
		//Z 1
		points[index++] = -dSize;
		points[nPoints - index] = -dSize;


		//X 2
		points[index++] = v;
		points[nPoints - index] = v;
		//Y 2
		points[index++] = 0.0f;
		points[nPoints - index] = 0.0f;
		// Z 2
		points[index++] = dSize;
		points[nPoints - index] = dSize;
	}

	glLineWidth(1.0);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, 0, points);
	

	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_LINES, 0, nLines * 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	

	delete[] points;
}

void CGLRenderer::DrawAxes(double len) {

	glLineWidth(3.0);

	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f); // X axis - red
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(len, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f); // Y axis - green
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, len, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f); // Z axis - blue
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, len);

	glEnd();
}

void CGLRenderer::DrawCube(double dSize) {

	const double halfSize = dSize / 2.0;
	float* vertices = new float[24];
	UINT* ind = new unsigned int[24];

	vertices[0] = -halfSize; vertices[1] = -halfSize; vertices[2] = halfSize;		//0					
	vertices[3] = halfSize; vertices[4] = -halfSize; vertices[5] = halfSize;		//1		  7--6				  			
	vertices[6] = halfSize; vertices[7] = halfSize; vertices[8] = halfSize;			//2		 /  /|
	vertices[9] = -halfSize; vertices[10] = halfSize; vertices[11] = halfSize;		//3		3--2 5
																					//		|  |/
	vertices[12] = -halfSize; vertices[13] = -halfSize; vertices[14] = -halfSize;	//4		0--1
	vertices[15] = halfSize; vertices[16] = -halfSize; vertices[17] = -halfSize;	//5
	vertices[18] = halfSize; vertices[19] = halfSize; vertices[20] = -halfSize;		//6
	vertices[21] = -halfSize; vertices[22] = halfSize; vertices[23] = -halfSize;	//7

	ind[0] = 0; ind[1] = 1; 
	ind[2] = 3; ind[3] = 2;
	ind[4] = 7; ind[5] = 6; 
	ind[6] = 4; ind[7] = 5;
	ind[8] = 0; ind[9] = 1; 
	
	ind[10] = 0; ind[11] = 3; ind[12] = 7; ind[13] = 4;
	ind[14] = 1; ind[15] = 2; ind[16] = 6; ind[17] = 5;
	

	glLineWidth(2.0);
	glPointSize(10.0);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);


	//glColor3f(0.5f, 0.5f, 0.5f);
	//glDrawElements(GL_QUAD_STRIP, 10, GL_UNSIGNED_INT, ind);
	//glDrawElements(GL_QUADS, 8, GL_UNSIGNED_INT, ind + 10);

	ind[0] = 0; ind[1] = 1;
	ind[2] = 0; ind[3] = 3;
	ind[4] = 0; ind[5] = 4;

	ind[6] = 1; ind[7] = 2;
	ind[8] = 1; ind[9] = 5;

	ind[10] = 2; ind[11] = 3;
	ind[12] = 2; ind[13] = 6;

	ind[14] = 3; ind[15] = 7;

	ind[16] = 4; ind[17] = 5;
	ind[18] = 4; ind[19] = 7;

	ind[20] = 5; ind[21] = 6;

	ind[22] = 6; ind[23] = 7;

	glColor3f(0.0f, 0.0f, 0.0f);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, ind);

	//glColor3f(1.0f, 0.0f, 0.0f);
	//glDrawArrays(GL_POINTS, 0, 8);

	glDisableClientState(GL_VERTEX_ARRAY);

	delete[] vertices;
	delete[] ind;
}

void CGLRenderer::DrawRobot() {


	/*
		Napisati funkciju CGLRenderer::DrawRobot(), koja robota. Za crtanje svih delova robota koristiti samo jedinicne 
		kocke (pozivom funkcije DrawCube(1.0), koja je definisana u prethodnom poglavlju). Pojedini delovi tela dobijaju se 
		primenom odgovarajucih transformacija (skaliranje/translacija/rotacija) na jedinicne kocke. Robota nacrtati kao zicani
		model sa sledecim dimenzijama delova: 
		- trup – 1.0 x 2.0 x 0.5 
		- nadlaktice i podlaktice – 0.3 x 1.0 x 0.5 
		- glava – 0.6 x 0.6 x 0.6 
	
		U klasi CGLView dodati funkciju koja ce reagovati na pritisak tastera na tastaturi, odnosno rukovati porukom WM_KEYDOWN.
		Ova funkcija treba da pozove funkciju CGLRenderer::OnKeyDown(UINT nChar) i prosledi joj pritisnuti taster.

		Funkcija CGLRenderer::OnKeyDown() treba da izvrsi sledece operacije: 
		-ako je pritisnut taster Q da rotira telo robota ulevo, 
		-ako je pritisnut taster W da rotira telo robota udesno, 
		-ako je pritisnut taster E da rotira glavu robota ulevo, 
		-ako je pritisnut taster R da rotira glavu robota udesno, 
		-ako je pritisnut taster A da rotira levu ruku robota u ramenu navise, 
		-ako je pritisnut taster Z da rotira levu ruku robota u ramenu nanize, 
		-ako je pritisnut taster S da rotira desnu ruku robota u ramenu navise, 
		-ako je pritisnut taster X da rotira desnu ruku robota u ramenu nanize, 
		-ako je pritisnut taster D da rotira levu ruku robota u laktu navise, 
		-ako je pritisnut taster C da rotira levu ruku robota u laktu nanize, 
		-ako je pritisnut taster F da rotira desnu ruku robota u laktu navise, 
		-ako je pritisnut taster V da rotira desnu ruku robota u laktu nanize.

		Korak rotacije neka bude 5 stepeni. Pri definisanju rotacije nije neophodno voditi racuna o ogranicavanju uglova do 
		kojih mogu da se rotiraju ruke robota. 
	*/

	glRotatef(bodyAngle, 0.0, 1.0, 0.0);
	glPushMatrix();

	//body
	glScalef(1.0, 2.0, 0.5);
	DrawCube(1.0);

	glPopMatrix();
	glPushMatrix();

	//right arm
	glTranslatef(-0.65, 1, 0);
	glRotatef(rightUpperArmAngle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, -0.5, 0.0);

	glPushMatrix();

	glScalef(0.3, 1.0, 0.5);
	DrawCube(1.0);

	glPopMatrix();

	glTranslatef(0.0, -0.5, 0.0);
	glRotatef(rightForearmAngle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, -0.5, 0.0);
	glScalef(0.3, 1.0, 0.5);
	DrawCube(1);

	glPopMatrix();
	glPushMatrix();

	//left arm
	//right arm
	glTranslatef(0.65, 1, 0);
	glRotatef(leftUpperArmAngle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, -0.5, 0.0);

	glPushMatrix();

	glScalef(0.3, 1.0, 0.5);
	DrawCube(1.0);

	glPopMatrix();

	glTranslatef(0.0, -0.5, 0.0);
	glRotatef(leftForearmAngle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, -0.5, 0.0);
	glScalef(0.3, 1.0, 0.5);
	DrawCube(1);

	glPopMatrix();

	//head
	glRotatef(headAngle, 0.0, 1.0, 0.0);
	glScalef(0.6, 0.6, 0.6);
	glTranslatef(0.0, 1.3 / 0.6, 0);
	DrawCube(1.0);

}

void CGLRenderer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
		case 'Q':
		case 'q':
		bodyAngle += 5;
		break;
		case 'W':
		case 'w':
		bodyAngle -= 5;
		break;
		case 'E':
		case 'e':
		headAngle += 5;
		break;
		case 'R':
		case 'r':
		headAngle -= 5;
		break;
		case 'A':
		case 'a':
		leftUpperArmAngle -= 5;
		break;
		case 'Z':
		case 'z':
		leftUpperArmAngle += 5;
		break;
		case 'S':
		case 's':
		rightUpperArmAngle -= 5;
		break;
		case 'X':
		case 'x':
		rightUpperArmAngle += 5;
		break;
		case 'D':
		case 'd':
		leftForearmAngle -= 5;
		break;
		case 'C':
		case 'c':
		leftForearmAngle += 5;
		break;
		case 'F':
		case 'f':
		rightForearmAngle -= 5;
		break;
		case 'V':
		case 'v':
		rightForearmAngle += 5;
		break;
	}
}

void CGLRenderer::SetCameraPosition() {

	float radYaw = DEG2RAD(camYaw);
	float radPitch = DEG2RAD(camPitch);

	float x = camDistance * cos(radPitch) * sin(radYaw);
	float y = camDistance * sin(radPitch);
	float z = camDistance * cos(radPitch) * cos(radYaw);

	gluLookAt(x, y, z,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f, 0.0f);
}

