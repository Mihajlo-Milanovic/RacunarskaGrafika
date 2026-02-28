#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

#define RAD(x) ((x)*3.14159265358979323846/180.0)

CGLRenderer::CGLRenderer(void)
{
	camYaw = 45.0f;
	camPitch = 30.0f;
	camDistance = 10.0f;
	isRotating = false;

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

	DrawGrid(10.0, 20);
	DrawAxes(5.0);
	DrawCube(1.0);

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
	UINT* ind = new UINT[24];
	float* normals = new float[72];

	vertices[0] = -halfSize; vertices[1] = -halfSize; vertices[2] = halfSize;		//0					
	vertices[3] = halfSize; vertices[4] = -halfSize; vertices[5] = halfSize;		//1		  7--6				  			
	vertices[6] = halfSize; vertices[7] = halfSize; vertices[8] = halfSize;			//2		 /  /|
	vertices[9] = -halfSize; vertices[10] = halfSize; vertices[11] = halfSize;		//3		3--2 5
																					//		|  |/
	vertices[12] = -halfSize; vertices[13] = -halfSize; vertices[14] = -halfSize;	//4		0--1
	vertices[15] = halfSize; vertices[16] = -halfSize; vertices[17] = -halfSize;	//5
	vertices[18] = halfSize; vertices[19] = halfSize; vertices[20] = -halfSize;		//6
	vertices[21] = -halfSize; vertices[22] = halfSize; vertices[23] = -halfSize;	//7

	//USING QUAD_STRIP FOR FRONT, TOP, BACK AND BOTTOM FACES
	
	//front
	ind[0] = 0; normals[0] = 0.0f; normals[1] = 0.0f; normals[2] = 1.0f;
	ind[1] = 1; normals[3] = 0.0f; normals[4] = 0.0f; normals[5] = 1.0f;
	ind[2] = 3; normals[6] = 0.0f; normals[7] = 0.0f; normals[8] = 1.0f;
	ind[3] = 2; normals[9] = 0.0f; normals[10] = 0.0f; normals[11] = 1.0f;
	
	//top
				normals[12] = 0.0f; normals[13] = 1.0f; normals[14] = 0.0f;
				normals[15] = 0.0f; normals[16] = 1.0f; normals[17] = 0.0f;
	ind[4] = 7; normals[18] = 0.0f; normals[19] = 1.0f; normals[20] = 0.0f;
	ind[5] = 6; normals[21] = 0.0f; normals[22] = 1.0f; normals[23] = 0.0f;

	//back
				normals[24] = 0.0f; normals[25] = 0.0f; normals[26] = -1.0f;
				normals[27] = 0.0f; normals[28] = 0.0f; normals[29] = -1.0f;
	ind[6] = 4; normals[30] = 0.0f; normals[31] = 0.0f; normals[32] = -1.0f;
	ind[7] = 5; normals[33] = 0.0f; normals[34] = 0.0f; normals[35] = -1.0f;

	//bottom
				normals[36] = 0.0f; normals[37] = -1.0f; normals[38] = 0.0f;
				normals[39] = 0.0f; normals[40] = -1.0f; normals[41] = 0.0f;
	ind[8] = 0; normals[42] = 0.0f; normals[43] = -1.0f; normals[44] = 0.0f;
	ind[9] = 1; normals[45] = 0.0f; normals[46] = -1.0f; normals[47] = 0.0f;

	//USING QUADS FOR LEFT AND RIGHT FACES

	//left
	ind[10] = 0; normals[48] = -1.0f; normals[49] = 0.0f; normals[50] = 0.0f;
	ind[11] = 3; normals[51] = -1.0f; normals[52] = 0.0f; normals[53] = 0.0f;
	ind[12] = 7; normals[54] = -1.0f; normals[55] = 0.0f; normals[56] = 0.0f;
	ind[13] = 4; normals[57] = -1.0f; normals[58] = 0.0f; normals[59] = 0.0f;

	//right
	ind[14] = 1; normals[60] = 1.0f; normals[61] = 0.0f; normals[62] = 0.0f;
	ind[15] = 2; normals[63] = 1.0f; normals[64] = 0.0f; normals[65] = 0.0f;
	ind[16] = 6; normals[66] = 1.0f; normals[67] = 0.0f; normals[68] = 0.0f;
	ind[17] = 5; normals[69] = 1.0f; normals[70] = 0.0f; normals[71] = 0.0f;


	glLineWidth(2.0);
	glPointSize(10.0);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glNormalPointer(GL_FLOAT, 0, normals);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);


	glColor3f(0.1f, 1.0f, 0.1f);
	GLfloat mat_diff[] = { 0.8, 0.8, 0.8, 0.1 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);

	//SetLighting();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glDrawElements(GL_QUAD_STRIP, 10, GL_UNSIGNED_INT, ind);
	glDrawElements(GL_QUADS, 8, GL_UNSIGNED_INT, ind + 10);

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

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


	glColor3f(1.0f, 0.0f, 0.0f);
	glDrawArrays(GL_POINTS, 0, 8);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	delete[] vertices;
	delete[] ind;
}

void CGLRenderer::SetCameraPosition() {

	float radYaw = RAD(camYaw);
	float radPitch = RAD(camPitch);

	float x = camDistance * cos(radPitch) * sin(radYaw);
	float y = camDistance * sin(radPitch);
	float z = camDistance * cos(radPitch) * cos(radYaw);

	gluLookAt(x, y, z,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f, 0.0f);
}

void CGLRenderer::SetLighting() {

	float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	float light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	float light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

}