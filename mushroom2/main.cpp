//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#pragma comment(lib, "GlAux.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glut.lib")

#define GLUT_DISABLE_ATEXIT_HACK 
#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include "../GL/glut.h"
#include "../GL/GLAux.h"
#include "mushroom.h"
#include <string>
#include <ctime>
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#define PI 3.14159265
   
float DEG_TO_RAD = 0.017453f;	
int Window_Width = 1366;//1366
int Window_Height = 768;//768

int timestate = 0;//
int Time_Move = 0;
int Time_Finish = 0;
///////////////////////////////wall
int L = 1000;			//wall length,1000
int H = 1618;			//wall width,1618
int WH = 600;			//wall height,1000
///////////////CAMERA/////////////
float eyePX = 0.0f;
float eyePY = 50.0f;
float eyePZ = 100.0f;

float viewDX = eyePX + 0.0f;
float viewDY = eyePY + 0.0f;
float viewDZ = eyePZ + (-1.0f);

float Dmove[3] = {0.0f, 0.0f, 0.0f};
vector<int> myvector;
bool Mark_Sleep = false;
bool Mark_Move = false;
bool Mark_Finish =  false;
bool Mark_cycle = false;
bool Mark_preset = false;
////////////////////////////////////////////////////////////
float lmodel_ambient[] = {0.0f, 0.0f, 1.0f, 1.0f};

GLfloat light_ambient[] = { 1.0f , 1.0f , 1.0f , 1.0f };
GLfloat light_diffuse[] = { 1.0f , 0.7f , 0.2f , 1.0f };
GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light_position[] = { 0.0f, 140.0f , 0.0f, 1.0f };
GLfloat light_spot_direction[] = {0.0f,1.0f , 0.0f};

/////////////////////////////////////////////////
GLfloat ambient[]   = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat diffuse[]   = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat emission[] = {0.0f, 0.0f, 0.0f, 1.0f};	
GLfloat shininess   = 0.0f;

////////////////////////////////////////////////////////////////////
GLuint ID[6];            //texture index
GLUquadricObj *quadric;	
/////////////////////////////////////////////////////////////////////
struct MUSHROOM
{
	float x, y, z;					//mushroom position
	int texT, texM, texB;			//mushroom texture
};
#define MAX_MUSHROOM_ORG 50			//max mushroom
int MAX_MUSHROOM = 50;
MUSHROOM* mushroom;

///////////////////////////////////////////////////////////////////////////
#define TM 20
#define TN 20
int TERRAIN_TRIANGLES = 50;
typedef struct TERRAIN_POINT_T
{
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
}TERRAIN_POINT;
TERRAIN_POINT terrain[TM+1][TN+1];
TERRAIN_POINT wall1[TM+1][TN+1];
TERRAIN_POINT wall2[TM+1][TN+1];

///////////////////////////////////////////////////////////////////////////////////
void SetTexture()
{   
	glShadeModel(GL_SMOOTH);
	_AUX_RGBImageRec *Image;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(50,ID); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	glEnable(GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glBindTexture( GL_TEXTURE_2D, ID[0]); 
	Image = auxDIBImageLoadA("mushtop2.bmp");
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Image->sizeX,Image->sizeY,GL_RGB,GL_UNSIGNED_BYTE,Image->data);

	glBindTexture( GL_TEXTURE_2D, ID[1]);
	Image = auxDIBImageLoadA("ground2.bmp");
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Image->sizeX,Image->sizeY,GL_RGB,GL_UNSIGNED_BYTE,Image->data);

	glBindTexture( GL_TEXTURE_2D, ID[2]);
	Image = auxDIBImageLoadA("wall.bmp");
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Image->sizeX,Image->sizeY,GL_RGB,GL_UNSIGNED_BYTE,Image->data);

	glBindTexture(GL_TEXTURE_2D, ID[3]);
	Image = auxDIBImageLoadA("treasure.bmp");
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Image->sizeX, Image->sizeY, GL_RGB, GL_UNSIGNED_BYTE, Image->data);

	glBindTexture( GL_TEXTURE_2D, ID[4]);
	Image = auxDIBImageLoadA("mushmiddle.bmp");
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Image->sizeX,Image->sizeY,GL_RGB,GL_UNSIGNED_BYTE,Image->data);

	glBindTexture( GL_TEXTURE_2D, ID[5]);
	Image = auxDIBImageLoadA("mushbottom.bmp");
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Image->sizeX,Image->sizeY,GL_RGB,GL_UNSIGNED_BYTE,Image->data);
}

void init_chopper()
{
	glShadeModel (GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	const GLfloat light_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
	const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_position[] = { 0.0f, -500.0f, 5.0f, 1.0f };

	const GLfloat mat_ambient[]    = { 0.5f, 0.5f, 0.5f, 1.0f };
	const GLfloat mat_diffuse[]    = { 0.5f, 0.5f, 0.5f, 1.0f };
	const GLfloat mat_specular[]   = { 0.3f, 0.3f, 0.3f, 1.0f };
	const GLfloat high_shininess[] = { 1000.0f };

	glEnable(GL_LIGHT2);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT2, GL_AMBIENT,  light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
}

void SetLight()
{
glLightfv(GL_LIGHT0 , GL_AMBIENT , light_ambient);
glLightfv(GL_LIGHT0 , GL_DIFFUSE , light_diffuse);
glLightfv(GL_LIGHT0 , GL_POSITION , light_position);
glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,4.0f);	
glEnable(GL_LIGHT0);
}

void chopper()
{
	init_chopper();
	int k = 50;
	//Chopper's head
	glPushMatrix();
	glTranslated(0.0, 0.2*k, 0.0);
	glRotated(90,0,0,1);
	glColor3d(0.894, 0.627, 0.337);
	glScaled(1.0,1.0,1.0);
	glutSolidSphere(0.25*k, 40, 40);
	glPopMatrix();

	//Chopper's eyes
	glPushMatrix();
	glTranslated(0.105*k, 0.2*k, 0.225*k);
	glRotated(20,0,1,0);
	glColor3d(1.0, 1.0, 1.0);
	glScaled(1.0,1.0,0.16);
	glutSolidSphere(0.045*k, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.105*k, 0.2*k, 0.225*k);
	glRotated(-20,0,1,0);
	glColor3d(1.0, 1.0, 1.0);
	glScaled(1.0,1.0,0.16);
	glutSolidSphere(0.045*k, 40, 40);
	glPopMatrix();

	//Chopper's eyes
	glPushMatrix();
	glTranslated(0.105*k, 0.2*k, 0.225*k);
	glRotated(20,0,1,0);
	glColor3d(0.0, 0.0, 0.0);
	glScaled(1.0,1.0,0.45);
	glutSolidSphere(0.03*k, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.105*k, 0.2*k, 0.225*k);
	glRotated(-20,0,1,0);
	glColor3d(0.0, 0.0, 0.0);
	glScaled(1.0,1.0,0.45);
	glutSolidSphere(0.03*k, 40, 40);
	glPopMatrix();

	//Chopper's nose
	glPushMatrix();
	glTranslated(0, 0.15*k, 0.216*k);
	//glRotated(-20,0,1,0);
	glColor3d(0.0666, 0.109, 0.9176);
	glScaled(1.0,1.0,1.6);
	glutSolidSphere(0.036*k, 40, 40);
	glPopMatrix();

	//Chopper's mouth
	glPushMatrix();
	glTranslated(0.0, 0.135*k, 0.2215*k);
	glRotated(12.6,1,0,0);
	glColor3d(0.854, 0.095, 0.11);
	glScaled(0.8,1.3,0.3);
	glutSolidTorus(0.039*k,0.04*k,40,40);
	glPopMatrix();

	//Chopper's hat
	glPushMatrix();
	glTranslated(0.0, 0.31*k, 0.0);
	glRotated(90,1,0,0);
	glColor3d(1, 0.4, 0.4);
	glScaled(1.0,1.0,0.9);
	glutSolidTorus(0.056*k,0.30,40,40);
	glPopMatrix();

	glPushMatrix();
	GLUquadricObj *quadratic;
	quadratic=gluNewQuadric();
	glTranslated(0.0, 0.70*k, 0.0);
	glColor3d(1, 0.4, 0.4);
	glRotated(90,1,0,0);
	gluCylinder(quadratic,0.27f*k,0.245f*k,0.38f*k,32,32);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 0.698*k, 0.0);
	glColor3d(1, 0.4, 0.4);
	glScaled(1.0,0.12,1.0);
	glutSolidSphere(0.27*k,30,30);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 0.505*k, 0.254*k);
	glColor3d(1.0, 1.0, 1.0);
	glRotated(2.25,1,0,0);
	glRotated(45,0,0,1);
	glScaled(0.048,0.2,0.014);
	glutSolidCube (1*k);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 0.505*k, 0.254*k);
	glColor3d(1.0, 1.0, 1.0);
	glRotated(2.25,1,0,0);
	glRotated(-45,0,0,1);
	glScaled(0.048,0.2,0.014);
	glutSolidCube (1*k);
	glPopMatrix();

	//Chopper's body
	glPushMatrix();
	glTranslated(0.0, -0.17*k, 0.0);
	glRotated(90,0,0,1);
	glColor3d(0.894, 0.627, 0.337);
	glScaled(1.0,1.0,1.0);
	glutSolidSphere(0.20*k, 40, 40);
	glPopMatrix();

	//Chopper's left arm
	glPushMatrix();
	if (Mark_Finish == false && Mark_preset == true) glRotated(30.0f*sin(Time_Move / 5.0 + 3.14), 1, 0, 0);
	glTranslated(0.12*k, -0.1*k, 0.0);
	glRotated(40,0,0,1);
	glPushMatrix();
	glColor3d(0.894, 0.627, 0.337);
	glScaled(0.5,2.7,0.5);
	glutSolidSphere(0.10*k, 40, 40);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.0, -0.26*k, 0.0);
	glColor3d(0.894, 0.627, 0.337);
	glScaled(1.0,1.0,1.0);
	glutSolidSphere(0.05*k, 40, 40);
	glPopMatrix();
	glPopMatrix();

	//Chopper's right arm
	glPushMatrix();
	if (Mark_Finish == false && Mark_preset == true) glRotated(30.0f*sin(Time_Move / 5.0), 1, 0, 0);
	glTranslated(-0.12*k, -0.1*k, 0.0);
	glRotated(-40,0,0,1);
	glPushMatrix();
	glColor3d(0.894, 0.627, 0.337);
	glScaled(0.5,2.7,0.5);
	glutSolidSphere(0.10*k, 40, 40);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.0, -0.26*k, 0.0);
	glColor3d(0.894, 0.627, 0.337);
	glScaled(1.0,1.0,1.0);
	glutSolidSphere(0.05*k, 40, 40);
	glPopMatrix();
	glPopMatrix();

	//Chopper's right leg
	glPushMatrix();
	if (Mark_Finish == false && Mark_preset == true)  glRotated(30.0f*sin(Time_Move / 5.0 + 3.14), 1, 0, 0);
	glTranslated(-0.08*k, -0.24*k, 0.0);
	glPushMatrix();
	glScaled(0.6,2.2,0.6);
	glColor3d(0.894, 0.627, 0.337);
	glutSolidSphere(0.10*k, 40, 40);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, -0.22*k, 0.04*k);
	glScaled(1.0,1.0,1.6);
	glColor3d(0.623, 0.308, 0.05);
	glutSolidSphere(0.05*k, 40, 40);
	glPopMatrix();
	glPopMatrix();

	//Chopper's left leg
	glPushMatrix();
	if (Mark_Finish == false && Mark_preset == true) glRotated(30.0f*sin(Time_Move / 5.0), 1, 0, 0);
	glTranslated(0.08*k, -0.24*k, 0.0);
	glPushMatrix();
	glScaled(0.6,2.2,0.6);
	glColor3d(0.894, 0.627, 0.337);
	glutSolidSphere(0.10*k, 40, 40);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, -0.22*k, 0.04*k);
	glScaled(1.0,1.0,1.6);
	glColor3d(0.623, 0.308, 0.05);
	glutSolidSphere(0.05*k, 40, 40);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void TerrainInit(TERRAIN_POINT data[TM+1][TN+1], int xLength, int yLength, int no, float step) 
{
	float cellx = 2*xLength/TM;
	float cellz = 2*yLength/TN;
	float* p1, *p2, *p3;
	for (int i=0;i<TM+1;i++)
	{
		for (int j=0;j<TN+1;j++)
		{
			data[i][j].x = cellx*i;
			data[i][j].y = float(rand()%no)*step;
			data[i][j].z = cellz*j;
			data[i][j].nx = 0.0f;
			data[i][j].ny = 0.0f;
			data[i][j].nz = 0.0f;
		}
	}
}

void MushroomInit()
{
	mushroom = new MUSHROOM[MAX_MUSHROOM];
	for (int i=0;i<MAX_MUSHROOM;i++)
	{
		mushroom[i].x = rand()%(2*(L-50))-(L-50);
		mushroom[i].y = rand()%30+5;
		mushroom[i].z = rand()%(2*(H-50))-(H-50);
		while ((-120.0f < mushroom[i].x) && (mushroom[i].x <120.0f)
			&& (-120.0f < mushroom[i].z) && (-120.0f < mushroom[i].z))
		{
			mushroom[i].x = rand()%(2*(L-50))-(L-50);
			mushroom[i].z = rand()%(2*(H-50))-(H-50);
		}
		mushroom[i].texT = 0;
		mushroom[i].texM = 4;
		mushroom[i].texB = 5;
	}
}

void Init()                    
{
   glDepthFunc(GL_LEQUAL);	
   glEnable(GL_DEPTH_TEST);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glEnable(GL_LINE_SMOOTH);
   glEnable(GL_BLEND);
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING);
   glShadeModel(GL_SMOOTH);		
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
   srand((unsigned)time(NULL));
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   SetTexture();
   TerrainInit(terrain,L,H,50,0.5f);
   TerrainInit(wall1,L,WH,50,1.0f);
   TerrainInit(wall2,H,WH,50,1.0f);
   SetLight();
   MushroomInit();
   quadric = gluNewQuadric();	
   gluQuadricTexture(quadric, GLU_TRUE);								
   gluQuadricDrawStyle(quadric, GLU_FILL);	
   gluQuadricNormals(quadric,GLU_SMOOTH);
   glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
}

void Reshape(int width, int height)                   
{
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));				
	glMatrixMode(GL_PROJECTION);										
	glLoadIdentity();													
	gluPerspective(45,(float)width/(float)height,0.1,10080);			
	glMatrixMode(GL_MODELVIEW);											
	glLoadIdentity();													
}

void DrawBottom()
{	
	   glClear(GL_DEPTH_BUFFER_BIT);
	   glEnable(GL_TEXTURE_2D);
	   glBindTexture(GL_TEXTURE_2D, ID[1]);

	   glTranslatef(-L,0.0f,-H);
	   glBegin(GL_TRIANGLES);
			  glNormal3f(0.0f,1.0f,0.0f);
			  for (int i=0;i<TM;i++)
			  {
				  for (int j=0;j<TN;j++)
				  {
					glTexCoord2f(i*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&terrain[i][j]);
					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&terrain[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&terrain[i+1][j]);

					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&terrain[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&terrain[i+1][j]);
					glTexCoord2f((i+1)*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&terrain[i+1][j+1]);
				  }
			  }
	   glEnd();
	  
	   glTranslatef(L,0.0f,H);

	   if (Time_Finish == 0)
		   glBindTexture(GL_TEXTURE_2D, ID[2]);
	   else
		   glBindTexture(GL_TEXTURE_2D, ID[3]);

	   glScalef(1.0f, -1.0f, 1.0f);
	   glNormal3f(0.0f,1.0f,-1.0f);
	   glPushMatrix();
	   glTranslatef(L,0.0f,H);
	   glRotatef(-180.0f,0.0f,1.0f,0.0f);
	   glRotatef(90.0f,1.0f,0.0f,0.0f);
	   glTranslatef(0.0f,0.0f,-WH);
	   glBegin(GL_TRIANGLES);
			  for (int i=0;i<TM;i++)
			  {
				  for (int j=0;j<TN;j++)
				  {
					glTexCoord2f(i*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i][j]);
					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i+1][j]);

					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i+1][j]);
					glTexCoord2f((i+1)*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i+1][j+1]);
				  }
			  }
	   glEnd();
	   glPopMatrix();

	   glNormal3f(0.0f,0.0f,1.0f);
	   glPushMatrix();
	   glTranslatef(-L,0.0f,-H);	   
	   glRotatef(90.0f,1.0f,0.0f,0.0f);
	   glTranslatef(0.0f,0.0f,-WH);
	   glBegin(GL_TRIANGLES);
			  for (int i=0;i<TM;i++)
			  {
				  for (int j=0;j<TN;j++)
				  {
					glTexCoord2f(i*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i][j]);
					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i+1][j]);

					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i+1][j]);
					glTexCoord2f((i+1)*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall1[i+1][j+1]);
				  }
			  }
	   glEnd();
	   glPopMatrix();

	   glNormal3f(-1.0f,0.0f,0.0f);
	   glPushMatrix();
	   glTranslatef(L,0.0f,-H);	  
	   glRotatef(-90.0f,0.0f,1.0f,0.0f);
	   glRotatef(90.0f,1.0f,0.0f,0.0f);
	   glTranslatef(0.0f,0.0f,-WH);
	   glBegin(GL_TRIANGLES);
			  for (int i=0;i<TM;i++)
			  {
				  for (int j=0;j<TN;j++)
				  {
					glTexCoord2f(i*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i][j]);
					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i+1][j]);

					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i+1][j]);
					glTexCoord2f((i+1)*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i+1][j+1]);
				  }
			  }
	   glEnd();
	   glPopMatrix();

	   glNormal3f(1.0f,0.0f,0.0f);
	   glPushMatrix();
	   glTranslatef(-L,0.0f,H);	  
	   glRotatef(90.0f,0.0f,1.0f,0.0f);
	   glRotatef(90.0f,1.0f,0.0f,0.0f);
	   glTranslatef(0.0f,0.0f,-WH);
	   glBegin(GL_TRIANGLES);
			  for (int i=0;i<TM;i++)
			  {
				  for (int j=0;j<TN;j++)
				  {
					glTexCoord2f(i*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i][j]);
					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i+1][j]);

					glTexCoord2f(i*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i][j+1]);
					glTexCoord2f((i+1)*1.0f/TM, j*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i+1][j]);
					glTexCoord2f((i+1)*1.0f/TM, (j+1)*1.0f/TN);glVertex3fv((GLfloat*)&wall2[i+1][j+1]);
				  }
			  }
	   glEnd();
	   glPopMatrix();
}

void drawCube(float l, float w, float h)          
{
	glBegin(GL_QUADS);
	glVertex3f(-l*0.5,+0.5*w,+0.5*h);
	glVertex3f(-l*0.5,+0.5*w,-0.5*h);
	glVertex3f(-l*0.5,-0.5*w,-0.5*h);
	glVertex3f(-l*0.5,-0.5*w,+0.5*h);

	glVertex3f(-l*0.5,+0.5*w,-0.5*h);
	glVertex3f(+l*0.5,+0.5*w,-0.5*h);
	glVertex3f(+l*0.5,-0.5*w,-0.5*h);
	glVertex3f(-l*0.5,-0.5*w,-0.5*h);

	glVertex3f(+l*0.5,+0.5*w,-0.5*h);
	glVertex3f(+l*0.5,+0.5*w,+0.5*h);
	glVertex3f(+l*0.5,-0.5*w,+0.5*h);
	glVertex3f(+l*0.5,-0.5*w,-0.5*h);

	glVertex3f(+l*0.5,+0.5*w,+0.5*h);
	glVertex3f(-l*0.5,+0.5*w,+0.5*h);
	glVertex3f(-l*0.5,-0.5*w,+0.5*h);
	glVertex3f(+l*0.5,-0.5*w,+0.5*h);

	glVertex3f(+l*0.5,+0.5*w,-0.5*h);
	glVertex3f(-l*0.5,+0.5*w,-0.5*h);
	glVertex3f(-l*0.5,+0.5*w,+0.5*h);
	glVertex3f(+l*0.5,+0.5*w,+0.5*h);

	glVertex3f(+l*0.5,-0.5*w,+0.5*h);
	glVertex3f(+l*0.5,-0.5*w,-0.5*h);
	glVertex3f(-l*0.5,-0.5*w,-0.5*h);
	glVertex3f(-l*0.5,-0.5*w,+0.5*h);
	glEnd();
}

GLfloat		angle;
void DrawMushroom(float x, float y, float z, int iTexTop, int iTexMiddle, int iTexBottom)
{
	glTranslatef(0,-24,0);
	float factor = float(y+24)/20;
	glTranslatef(x,10*factor,z);
	glScalef(factor,factor,factor);
	glBindTexture(GL_TEXTURE_2D, ID[iTexTop]);
	float q3 = 1.73205080;
	float r = 3*q3;
	float R = 4*q3+4/q3;
	float h = 3 + 4;
	float q5 = 2.23606798;
	glRotatef(-90,1.0f,0.0f,0.0f);
	gluCylinder(quadric, R, r, h, 32, 32);
	glTranslatef(0,0,4);
	gluSphere(quadric,6,32,32);
	
	glBindTexture(GL_TEXTURE_2D, ID[iTexMiddle]);
	glTranslatef(0,0,-4);
	gluQuadricOrientation(quadric,GLU_INSIDE);
	gluDisk(quadric,R - 2*q5-0.3,R,32,32);
	gluQuadricOrientation(quadric,GLU_OUTSIDE);

	glBindTexture(GL_TEXTURE_2D, ID[iTexBottom]);
	glTranslatef(0,0,-8);
	gluCylinder(quadric, 2*q5-0.5, 2*q5, 8,32,32);
	gluSphere(quadric,2*q5-0.5,32,32);
}

void Display2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();
		if (Mark_preset == false)
		{
			float VDX = 360 * sin(min(timestate / 2.0, 360.0)*PI / 180);
			float VDZ = 360 * cos(min(timestate / 2.0, 360.0)*PI / 180 + PI);
			gluLookAt(eyePX, eyePY, eyePZ, VDX, viewDY, VDZ, 0, 1, 0);
			if (min(timestate / 2.0, 360.0) == 360) { Mark_preset = true;  }
		}
		else
		{
			gluLookAt(eyePX, eyePY, eyePZ, viewDX, viewDY, viewDZ, 0, 1, 0);
		}
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glPushMatrix();
	init_chopper();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, -24, 0);
	DrawBottom();          
	glPopMatrix();
	
	glPushMatrix();
	GLdouble displacement = -800.0;
	glTranslated(0.0f, -10.0f, displacement);

	if (Mark_preset == true)
	{
		if (myvector.size() < 3 || Time_Move < 100)
		{
			if (Mark_Move == false)
			{
				Time_Move = 0;
				if (myvector.size() > 0)
				{
					unsigned index_vector = myvector.at(myvector.size() - 1);
					glTranslated(mushroom[index_vector].x, 20, mushroom[index_vector].z);
				}

				double min_dist = 0.0;
				int num, i;
				for (i = 1; i < MAX_MUSHROOM; i++)//start from mushroom 0; firefly flies to M0
				{
					if (find(myvector.begin(), myvector.end(), i) == myvector.end())
					{
						double dist;
						if (myvector.size() > 1)
						{
							dist = sqrt((mushroom[i].x - Dmove[0]) * (mushroom[i].x - Dmove[0]) + (mushroom[i].z - Dmove[2])*(mushroom[i].z - Dmove[2]));
						} 
						else
						{
							dist = sqrt((mushroom[i].x - Dmove[0]) * (mushroom[i].x - Dmove[0]) + (mushroom[i].z - Dmove[2] - displacement)*(mushroom[i].z - Dmove[2] - displacement));
						}
						if (min_dist == 0)
						{
							min_dist = dist; num = i;
						}
						else
						{
							if (min_dist > dist)
							{
								min_dist = dist; num = i;
							}
						}
					}
				}
				myvector.push_back(num);
				Dmove[0] = mushroom[num].x;
				Dmove[1] = mushroom[num].y;
				Dmove[2] = mushroom[num].z;
				Mark_Move = true;
			}
			else
			{
				unsigned index_vector;
				if (myvector.size() > 1)
					index_vector = myvector.at(myvector.size() - 2);
				Time_Move = min(Time_Move, 100);
				if (myvector.size() == 1)
				{
					glTranslated((Dmove[0] / 100)*Time_Move, 20, ((Dmove[2]-displacement) / 100)*Time_Move);
				}
				else
				{
					float DMX = mushroom[index_vector].x + ((Dmove[0] - mushroom[index_vector].x) / 100)*Time_Move;
					float DMZ = (mushroom[index_vector].z - displacement) + ((Dmove[2] - mushroom[index_vector].z) / 100)*Time_Move;
					glTranslated(DMX, 20, DMZ);
				}
				if (Time_Move == 100)
					Mark_Move = false;
			}
		}
		else
		{
			Time_Finish = min(Time_Finish, 100);
			float DMX = Dmove[0] + ((-Dmove[0]) / 100)*Time_Finish;
			float DMZ = (Dmove[2] - displacement) + ((-Dmove[2]) / 100)*Time_Finish;
			glTranslated(DMX, 20, DMZ);
			Time_Finish++;
			if (Time_Finish == 100)
			{
				Mark_Finish = true;
			}
		}
		if (myvector.size() == 1)
		{
			float theta_R = atan2(Dmove[0], Dmove[2]-displacement) * 180 / PI;
			glRotatef(theta_R, 0.0f, 1.0f, 0.0f);
		}
		else
		{
			if (Time_Finish != 0)
			{
				float theta_R = atan2((Dmove[0]), (Dmove[2])) * 180 / PI + 180.0;
				glRotatef(theta_R, 0.0f, 1.0f, 0.0f);
			}
			else
			{
				unsigned index_vector = myvector.at(myvector.size() - 2);
				float theta_R = atan2((Dmove[0] - mushroom[index_vector].x), (Dmove[2] - mushroom[index_vector].z)) * 180 / PI;
				glRotatef(theta_R, 0.0f, 1.0f, 0.0f);
			}
		}
	}
	chopper();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	for (int i = 1; i < MAX_MUSHROOM; i++)
	{
		if (myvector.size() > 1 )
		{
			if (Time_Finish != 0)
			{
				if (find(myvector.begin(), myvector.end(), i) == myvector.end())
				{
					glPushMatrix();
					DrawMushroom(mushroom[i].x, mushroom[i].y, mushroom[i].z, mushroom[i].texT, mushroom[i].texM, mushroom[i].texB);
					glPopMatrix();
				}
			}
			else
			{
				if (find(myvector.begin(), myvector.end() - 1, i) == myvector.end() - 1)
				{
					glPushMatrix();
					DrawMushroom(mushroom[i].x, mushroom[i].y, mushroom[i].z, mushroom[i].texT, mushroom[i].texM, mushroom[i].texB);
					glPopMatrix();
				}
			}

		}
		else
		{
			glPushMatrix();
			DrawMushroom(mushroom[i].x, mushroom[i].y, mushroom[i].z, mushroom[i].texT, mushroom[i].texM, mushroom[i].texB);
			glPopMatrix();
		}
	}



	timestate++;
	Time_Move++;
	glutSwapBuffers();
	if (Mark_Sleep == false)
	{
		Sleep(5000);
		Mark_Sleep = true;
	}
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);         
   glutInitWindowSize(Window_Width, Window_Height);             
   glutInitWindowPosition(0, 0);        
   glutCreateWindow(argv[0]);          
   Init();
   glutReshapeFunc(Reshape);
   glutDisplayFunc(Display2);
   glutIdleFunc(idle);
   glutMainLoop();             
   return 0;
}