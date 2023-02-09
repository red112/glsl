#include <iostream>

#include <gl/glew.h>
#include <GL/glut.h>
#include "textfile.h"
#include <math.h>

#define PI 3.141592

void initGL();					//opengl 초기화

//GLUT
void changeSize(int w, int h);	//윈도우 크기 변경 시 호출되는 callback

//Rendering
void display();				//기본 랜더링 코드
//float rotate_angle = 0.f;	//주전자 회전 animatio용 각도 회전 각도 변수

//GLEW
void initGLEW();			//GLEW 초기화

//Shader
GLuint v_shader;			//vertex shader handle
GLuint f_shader;			//fragment shader handle
GLuint program_shader;		//shader program handle
void setShaders();			//Shader 설정

// 회전 Animation
GLint loc = 0;


//Logging
#define printOpenGLError() printOglError(__FILE__, __LINE__)
int printOglError(char* file, int line);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

float slDir[3] = { 0.f, 0.f, -1.f };		//Spot light 방향
float lpos[4] = { 0.f, 0.f, 1.f, 1.f };
bool bIncrease = true;

void DrawSphere(float rad, int numLatitude, int numLongitude);
void LightInit();

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(300, 300);
	glutCreateWindow("OpenGL");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutIdleFunc(display);

	initGLEW();
	setShaders();

	initGL();

    glutMainLoop();
    return 0;
}


//////////////////////////
void changeSize(int w, int h) {
	if (h == 0)	h = 1;
	float ratio = 1.f * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
}


void initGL()
{
	LightInit();

	//Modelview and projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	//Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw
	//glColor3f(0.5f, 0.5f, 1.f);
	//glUniform1fARB(loc, rotate_angle);
	//glPushMatrix();
	//glRotatef(rotate_angle, 0.f, 1.f, 0.f);
	glutSolidTeapot(0.5f);
	//DrawSphere(0.8f, 40, 40);
	//glPopMatrix();
	glFlush();

	glutSwapBuffers();
	//rotate_angle = rotate_angle + 0.001f;
	//if (rotate_angle > 360.f) rotate_angle -= 360.f;
	float route_len = 1.f;
	if (bIncrease)
	{
		lpos[0] = lpos[0] + 0.0002f;
		if (lpos[0] >= route_len)
		{
			lpos[0] = route_len;	bIncrease = false;
		}
	}
	else
	{
		lpos[0] = lpos[0] - 0.0002f;
		if (lpos[0] <= -route_len)
		{
			lpos[0] = -route_len;	bIncrease = true;
		}
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	

}


void initGLEW()
{
	//Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(0);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	//Check Shader
	//ARB
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL (ARB)\n");
	else
	{
		printf("No GLSL support\n");
		exit(0);
	}

	//OpenGL 4.6
	if (glewIsSupported("GL_VERSION_4_6"))
		printf("Ready for OpenGL 4.6\n");
	else {
		printf("OpenGL 4.6 not supported\n");
		exit(0);
	}
}

//GLuint v_shader, f_shader, program_shader
void setShaders()
{
	char* vs = NULL, * fs = NULL, * fs2 = NULL;

	v_shader = glCreateShader(GL_VERTEX_SHADER);
	f_shader = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead((char*)"minimal.vert");
	fs = textFileRead((char*)"minimal.frag");

	const char* vv = vs;
	const char* ff = fs;

	glShaderSource(v_shader, 1, &vv, NULL);
	glShaderSource(f_shader, 1, &ff, NULL);

	free(vs); free(fs);

	glCompileShader(v_shader);
	glCompileShader(f_shader);

	printShaderInfoLog(v_shader);
	printShaderInfoLog(f_shader);

	program_shader = glCreateProgram();
	glAttachShader(program_shader, v_shader);
	glAttachShader(program_shader, f_shader);

	glLinkProgram(program_shader);
	printProgramInfoLog(program_shader);

	glUseProgram(program_shader);

	//animation용 회전값 핸들 설정
	//loc = glGetUniformLocationARB(program_shader, "angle");

	//loc = glGetUniformLocationARB(program_shader, "material_diffuse");
	//glUniform4fARB(loc, material_diffuse[0], material_diffuse[1], material_diffuse[2],material_diffuse[3]);
}


#define printOpenGLError() printOglError(__FILE__, __LINE__)
int printOglError(char* file, int line)
{
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}



void DrawSphere(float rad, int numLatitude, int numLongitude)
{	

	//최소한 육면체 이상은 되어야 하므로 위도는 2개, 경도는 3개를 최소로 한다.
	if (numLatitude < 2)	numLatitude = 2;
	if (numLongitude < 3)	numLongitude = 3;

	//삼각함수를 쓰기 위해 Degree가 아닌 Radian을 쓰도록 한다.
	float angleStepLati = PI / (float)numLatitude;		//위도는 180도, 즉 PI를 나눈다.
	float angleStepLong = 2.f * PI / (float)numLongitude;	//경도는 360도, 즉 2*PI를 나눈다.


	float prjLen, prjLenNext;
	float p1[3], p2[3], p3[3], p4[3];
	float n1[3], n2[3], n3[3], n4[3];
	//위도
	for (int countLati = 0;countLati < numLatitude;countLati++)
	{
		p1[1] = p2[1] = rad * cos(angleStepLati * (float)countLati);			//기준 Y좌표.(위도)
		p3[1] = p4[1] = rad * cos(angleStepLati * (float)(countLati + 1));		//다음 Y좌표.(위도)

		prjLen = rad * sin(angleStepLati * (float)countLati);		//기준점의 XZ평면에 투영된 길이
		prjLenNext = rad * sin(angleStepLati * (float)(countLati + 1));	//다음점의 XZ평면에 투영된 길이

		for (int countLong = 0;countLong < numLongitude;countLong++)
		{
			p1[0] = prjLen * cos(angleStepLong * (float)countLong);
			p1[2] = prjLen * sin(angleStepLong * (float)countLong);

			p2[0] = prjLen * cos(angleStepLong * (float)(countLong + 1));
			p2[2] = prjLen * sin(angleStepLong * (float)(countLong + 1));

			p3[0] = prjLenNext * cos(angleStepLong * (float)countLong);
			p3[2] = prjLenNext * sin(angleStepLong * (float)countLong);

			p4[0] = prjLenNext * cos(angleStepLong * (float)(countLong + 1));
			p4[2] = prjLenNext * sin(angleStepLong * (float)(countLong + 1));

			memcpy(n1, p1, sizeof(float) * 3);
			memcpy(n2, p2, sizeof(float) * 3);
			memcpy(n3, p3, sizeof(float) * 3);
			memcpy(n4, p4, sizeof(float) * 3);

			//			glBegin(GL_LINE_LOOP);
			glBegin(GL_TRIANGLES);
			glNormal3fv(n1);
			glVertex3fv(p1);
			glNormal3fv(n3);
			glVertex3fv(p3);
			glNormal3fv(n2);
			glVertex3fv(p2);
			glEnd();

			//			glBegin(GL_LINE_LOOP);
			glBegin(GL_TRIANGLES);
			glNormal3fv(n2);
			glVertex3fv(p2);
			glNormal3fv(n3);
			glVertex3fv(p3);
			glNormal3fv(n4);
			glVertex3fv(p4);
			glEnd();

		}


	}

}



void LightInit()
{
	glClearColor(0.0f, .0f, .0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);


	float dif[4] = { 0.8f, 0.8f, 0.8f, 1.f };
	float amb[4] = { 0.0f, 0.f, 0.f, 1.f };
	float spc[4] = { 1.f, 1.f, 1.f, 1.f };

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spc);

	float material_amb_diffuse[4] = { 0.2f, 0.2f, 0.8f, 1.f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_amb_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_amb_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spc);
	glMateriali(GL_FRONT, GL_SHININESS, 128);


	// Set attenuation
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.8);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.9);

	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 25.f);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, slDir);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.f);
	
	//glEnable(GL_LIGHT0);

}