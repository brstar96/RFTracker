///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// drawing a icosahedron using vertex array (glDrawElements)
// dependency: freeglut/glut
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2018-07-17
// UPDATED: 2018-07-31
///////////////////////////////////////////////////////////////////////////////

#include "include\GL\freeglut.h"
//#include "include\GL\glut.h"


#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
//#include "Bmp.h"
#include "Icosphere.h"


// GLUT CALLBACK functions
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void keyboardCB(unsigned char key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);

void initGL();
int  initGLUT(int argc, char **argv);
bool initSharedMem();
void clearSharedMem();
//void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void toOrtho();
void toPerspective();
//GLuint loadTexture(const char* fileName, bool wrap = true);


// constants
const int   SCREEN_WIDTH = 500;
const int   SCREEN_HEIGHT = 500;
const float CAMERA_DISTANCE = 3.0f;
const int   TEXT_WIDTH = 8;
const int   TEXT_HEIGHT = 13;


// global variables
void *font = GLUT_BITMAP_8_BY_13;
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int drawMode;
GLuint texId;                       // texture object
int imageWidth;
int imageHeight;
int subdivision = 0;
//Icosphere sphere(1.0f);             // radius only
//Icosphere sphere(1.0f, 0);          // radius, subdivision
Icosphere sphere(1.0f, subdivision, false);    // radius, subdivision, smooth



											   ///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	// init global vars
	initSharedMem(); //스크린해상도, 마우스포지션, 카메라앵글 및 거리, 콘솔출력용 정보출력함수(printSelf())

	// init GLUT and GL
	initGLUT(argc, argv);
	initGL();

	// the last GLUT call (LOOP)
	// window will be shown and display callback is triggered by events
	// NOTE: this call never return main().
	glutMainLoop(); /* Start GLUT event-processing loop */

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char **argv)
{
	// GLUT stuff for windowing
	// initialization openGL window.
	// it is called before any other GLUT routine
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

	glutInitWindowSize(screenWidth, screenHeight);  // window size

	glutInitWindowPosition(100, 100);               // window location

													// finally, create a window with openGL context
													// Window will not displayed until glutMainLoop() is called
													// it returns a unique ID
	int handle = glutCreateWindow(argv[0]);     // param is the title of window

												// register GLUT callback functions
	glutDisplayFunc(displayCB);
	glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
	glutReshapeFunc(reshapeCB);

	glutKeyboardFunc(keyboardCB);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);

	return handle;
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
	//glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

												// enable /disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);

	glClearColor(0, 0, 0, 0);                   // background color
	glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);

	//initLights();
}



///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
	//glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glColor4fv(color);          // set text color
	glRasterPos2i(x, y);        // place text position

								// loop all characters in the string
	while (*str)
	{
		glutBitmapCharacter(font, *str);
		++str;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);

	glColor4fv(color);          // set text color
	glRasterPos3fv(pos);        // place text position

								// loop all characters in the string
	while (*str)
	{
		glutBitmapCharacter(font, *str);
		++str;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;

	mouseLeftDown = mouseRightDown = mouseMiddleDown = false;
	mouseX = mouseY = 0;

	cameraAngleX = cameraAngleY = 0.0f;
	cameraDistance = CAMERA_DISTANCE;

	//drawMode = 0; // 0:fill, 1: wireframe, 2:points

				  //DEBUG
				  //sphere.setEdgeLength(1);
	sphere.printSelf();

	return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up global vars
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
//void initLights()
//{
//	// set up light colors (ambient, diffuse, specular)
//	GLfloat lightKa[] = { .3f, .3f, .3f, 1.0f };  // ambient light
//	GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };  // diffuse light
//	GLfloat lightKs[] = { 1, 1, 1, 1 };           // specular light
//	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
//	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
//
//	// position the light
//	float lightPos[4] = { 0, 0, 1, 0 }; // directional light
//	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
//
//	glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
//}



///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}


//
/////////////////////////////////////////////////////////////////////////////////
//// load raw image as a texture
/////////////////////////////////////////////////////////////////////////////////
//GLuint loadTexture(const char* fileName, bool wrap)
//{
//	Image::Bmp bmp;
//	if (!bmp.read(fileName))
//		return 0;     // exit if failed load image
//
//					  // get bmp info
//	int width = bmp.getWidth();
//	int height = bmp.getHeight();
//	const unsigned char* data = bmp.getDataRGB();
//	GLenum type = GL_UNSIGNED_BYTE;    // only allow BMP with 8-bit per channel
//
//									   // We assume the image is 8-bit, 24-bit or 32-bit BMP
//	GLenum format;
//	int bpp = bmp.getBitCount();
//	if (bpp == 8)
//		format = GL_LUMINANCE;
//	else if (bpp == 24)
//		format = GL_RGB;
//	else if (bpp == 32)
//		format = GL_RGBA;
//	else
//		return 0;               // NOT supported, exit
//
//								// gen texture ID
//	GLuint texture;
//	glGenTextures(1, &texture);
//
//	// set active texture and configure it
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	// select modulate to mix texture with color for shading
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
//
//	// if wrap is true, the texture wraps over at the edges (repeat)
//	//       ... false, the texture ends at the edges (clamp)
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
//	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	// copy texture data
//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
//	//glGenerateMipmap(GL_TEXTURE_2D);
//
//	// build our texture mipmaps
//	switch (bpp)
//	{
//	case 8:
//		gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_LUMINANCE, type, data);
//		break;
//	case 24:
//		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, type, data);
//		break;
//	case 32:
//		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, type, data);
//		break;
//	}
//
//	return texture;
//}



///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
	// backup current model-view matrix
	glPushMatrix();                     // save current modelview matrix
	glLoadIdentity();                   // reset modelview matrix

										// set to 2D orthogonal projection
	glMatrixMode(GL_PROJECTION);        // switch to projection matrix
	glPushMatrix();                     // save current projection matrix
	glLoadIdentity();                   // reset projection matrix
										//gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection
	glOrtho(0, screenWidth, 0, screenHeight, -1, 1); // set to orthogonal projection

	float color[4] = { 1, 1, 1, 1 };

	std::stringstream ss;
	ss << std::fixed << std::setprecision(3);

	ss << "Subdivision: " << sphere.getSubdivision() << std::ends;
	drawString(ss.str().c_str(), 1, screenHeight - TEXT_HEIGHT, color, font);
	ss.str("");

	ss << "Radius: " << sphere.getRadius() << std::ends;
	drawString(ss.str().c_str(), 1, screenHeight - (2 * TEXT_HEIGHT), color, font);
	ss.str("");

	ss << "Triangle Count: " << sphere.getTriangleCount() << std::ends;
	drawString(ss.str().c_str(), 1, screenHeight - (3 * TEXT_HEIGHT), color, font);
	ss.str("");

	ss << "Vertex Count: " << sphere.getVertexCount() << std::ends;
	drawString(ss.str().c_str(), 1, screenHeight - (4 * TEXT_HEIGHT), color, font);
	ss.str("");

	ss << "Index Count: " << sphere.getIndexCount() << std::ends;
	drawString(ss.str().c_str(), 1, screenHeight - (5 * TEXT_HEIGHT), color, font);
	ss.str("");

	ss << "Press SPACE key to change subdivision level" << std::ends;
	drawString(ss.str().c_str(), 1, 1, color, font);
	ss.str("");

	// unset floating format
	ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

	// restore projection matrix
	glPopMatrix();                   // restore to previous projection matrix

									 // restore modelview matrix
	glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
	glPopMatrix();                   // restore to previous modelview matrix
}



///////////////////////////////////////////////////////////////////////////////
// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
void toOrtho()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

	// set orthographic viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void toPerspective()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)(screenWidth) / screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

																			   // switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}









//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB()
{
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();

	// tramsform camera
	glTranslatef(0, 0, -cameraDistance);
	glRotatef(cameraAngleX, 1, 0, 0);   // pitch
	glRotatef(cameraAngleY, 0, 1, 0);   // heading

										// transform model
	glRotatef(-90, 1, 0, 0);
	//glRotatef(-90, 0, 0, 1);

	// set material
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1 };
	float diffuse[] = { 0.8f, 0.8f, 0.8f, 1 };
	float specular[] = { 1.0f, 1.0f, 1.0f, 1 };
	float shininess = 128;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	// line color
	float lineColor[] = { 1.0f, 0.0f, 0.0f, 1 };

	// draw
	//glBindTexture(GL_TEXTURE_2D, texId);
	//glBindTexture(GL_TEXTURE_2D, 0);

	/*if (drawMode == 2)
		sphere.draw();
	else*/
		sphere.drawWithLines(lineColor);


	// unbind texture
	//glBindTexture(GL_TEXTURE_2D, 0);

	showInfo();     // print max range of glDrawRangeElements

	glPopMatrix();

	glutSwapBuffers();
}


void reshapeCB(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
	toPerspective();
	std::cout << "window resized: " << w << " x " << h << std::endl;

#ifdef _WIN32
	HWND handle = ::GetActiveWindow();
	RECT rect;
	::GetWindowRect(handle, &rect);
	std::cout << "window size: " << (rect.right - rect.left) << "x" << (rect.bottom - rect.top) << std::endl;
#endif
}


void timerCB(int millisec)
{
	glutTimerFunc(millisec, timerCB, millisec);
	glutPostRedisplay();
}


void keyboardCB(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESCAPE
		clearSharedMem();
		exit(0);
		break;

	case ' ': // space
		++subdivision;
		subdivision %= 7; // from 0 to 6
		sphere.setSubdivision(subdivision);
		break;

	//case 'd': // switch rendering modes (fill -> wire -> point)
	//case 'D':
	//	++drawMode;
	//	drawMode %= 3;
	//	if (drawMode == 0)        // fill mode
	//	{
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//		glEnable(GL_DEPTH_TEST);
	//		glEnable(GL_CULL_FACE);
	//	}
	//	else if (drawMode == 1)  // wireframe mode
	//	{
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//		glDisable(GL_DEPTH_TEST);
	//		glDisable(GL_CULL_FACE);
	//	}
	//	else                    // point mode
	//	{
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	//		glDisable(GL_DEPTH_TEST);
	//		glDisable(GL_CULL_FACE);
	//	}
		break;

	default:
		;
	}
}


void mouseCB(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}

	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}

	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseMiddleDown = true;
		}
		else if (state == GLUT_UP)
			mouseMiddleDown = false;
	}
}


void mouseMotionCB(int x, int y)
{
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if (mouseRightDown)
	{
		cameraDistance -= (y - mouseY) * 0.2f;
		mouseY = y;
	}
}
