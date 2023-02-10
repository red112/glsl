#include <stdio.h>
#include <stdlib.h>

#include <gl/glew.h>
#include <gl/glut.h>
#include "textfile.h"
#include <gl/glaux.h>
//#include <gl/glext.h>
#include <math.h>

#define PI 3.141592


bool bIncrease = true;
float lpos[4] = {	1.f, 1.f, 1.f, 0.f	};	//Directional light
float slDir[3] = {	0.f, 0.f, -1.f	};		//Spot light 방향


float p1[3] = { -0.8f,-0.8f,0.f};
float p2[3] = {  0.8f,-0.8f,0.f};
float p3[3] = {  0.8f, 0.8f,0.f};
float p4[3] = { -0.8f, 0.8f,0.f};

float n1[3] = { -1.f/(float)sqrt(3.0),-1.f/(float)sqrt(3.0),1.f/(float)sqrt(3.0)};
float n2[3] = {  1.f/(float)sqrt(3.0),-1.f/(float)sqrt(3.0),1.f/(float)sqrt(3.0)};
//float n2[3] = {  0.f,0.f,1.f};
//float n3[3] = {  0.f,0.f,1.f};
float n3[3] = {  1.f/(float)sqrt(3.0), 1.f/(float)sqrt(3.0),1.f/(float)sqrt(3.0)};
float n4[3] = { -1.f/(float)sqrt(3.0), 1.f/(float)sqrt(3.0),1.f/(float)sqrt(3.0)};


#define CUBE_LEN	0.5f

float c0[3] = { -CUBE_LEN,-CUBE_LEN,CUBE_LEN};
float c1[3] = { CUBE_LEN,-CUBE_LEN,CUBE_LEN};
float c2[3] = { CUBE_LEN,CUBE_LEN,CUBE_LEN};
float c3[3] = { -CUBE_LEN,CUBE_LEN,CUBE_LEN};
float c4[3] = { -CUBE_LEN,-CUBE_LEN,-CUBE_LEN};
float c5[3] = { CUBE_LEN,-CUBE_LEN,-CUBE_LEN};
float c6[3] = { CUBE_LEN,CUBE_LEN,-CUBE_LEN};
float c7[3] = { -CUBE_LEN,CUBE_LEN,-CUBE_LEN};

float t0[2] = { 0.f,0.f};
float t1[2] = { 1.f,0.f};
float t2[2] = { 1.f,1.f};
float t3[2] = { 0.f,1.f};

/////////////////////////// TEXTURE TGA 
typedef struct									// Create A Structure
{
	GLubyte	*imageData;							// Image Data (Up To 32 Bits)
	GLuint	bpp;								// Image Color Depth In Bits Per Pixel.
	GLuint	width;								// Image Width
	GLuint	height;								// Image Height
	GLuint	texID;								// Texture ID Used To Select A Texture
} TextureImage;

TextureImage textures[2];							// Storage For 2 Textures

bool LoadTGA(TextureImage *texture, char *filename)				// Loads A TGA File Into Memory
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	GLubyte		TGAcompare[12];						// Used To Compare TGA Header
	GLubyte		header[6];						// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;						// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;						// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;							// Temporary Variable
	GLuint		type=GL_RGBA;						// Set The Default GL Mode To RBGA (32 BPP)

	FILE *file = fopen(filename, "rb");					// Open The TGA File

	if(	file==NULL ||							// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0 ||		// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))		// If So Read Next 6 Header Bytes
	{
		if (file == NULL)						// Does The File Even Exist? *Added Jim Strong*
			return FALSE;						// Return False
		else								// Otherwise
		{
			fclose(file);						// If Anything Failed, Close The File
			return FALSE;						// Return False
		}
	}

	texture->width  = header[1] * 256 + header[0];				// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];				// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texture->width	<=0 ||						// Is The Width Less Than Or Equal To Zero
		texture->height	<=0 ||						// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))				// Is The TGA 24 or 32 Bit?
	{
		fclose(file);							// If Anything Failed, Close The File
		return FALSE;							// Return False
	}

	texture->bpp	= header[4];						// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;					// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture->width*texture->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture->imageData=(GLubyte *)malloc(imageSize);			// Reserve Memory To Hold The TGA Data

	if(	texture->imageData==NULL ||					// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture->imageData!=NULL)					// Was Image Data Loaded
			free(texture->imageData);				// If So, Release The Image Data

		fclose(file);							// Close The File
		return FALSE;							// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)			// Loop Through The Image Data
	{									// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texture->imageData[i];					// Temporarily Store The Value At Image Data 'i'
		texture->imageData[i] = texture->imageData[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
		texture->imageData[i + 2] = temp;				// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);								// Close The File

	// Build A Texture From The Data
	glGenTextures(1, &(texture->texID));					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture->texID);				// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	
	if (texture[0].bpp==24)							// Was The TGA 24 Bits
	{
		type=GL_RGB;							// If So Set The 'type' To GL_RGB
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture[0].imageData);

	return true;								// Texture Building Went Ok, Return True
}



/////////////////////////// TEXTURE TGA 


//GLuint  textures[2];
void	DrawCube()
{
	glBegin(GL_TRIANGLES);
	//+Z
		glNormal3f(0.f,0.f,1.f);
		glTexCoord2fv(t0);			glVertex3fv(c0);
		glTexCoord2fv(t1);			glVertex3fv(c1);
		glTexCoord2fv(t2);			glVertex3fv(c2);
		
		glTexCoord2fv(t0);			glVertex3fv(c0);
		glTexCoord2fv(t2);			glVertex3fv(c2);
		glTexCoord2fv(t3);			glVertex3fv(c3);
	//-Z
		glNormal3f(0.f,0.f,-1.f);
		glTexCoord2fv(t0);			glVertex3fv(c5);
		glTexCoord2fv(t1);			glVertex3fv(c4);
		glTexCoord2fv(t2);			glVertex3fv(c7);
		
		glTexCoord2fv(t0);			glVertex3fv(c5);
		glTexCoord2fv(t2);			glVertex3fv(c7);
		glTexCoord2fv(t3);			glVertex3fv(c6);
	//+X
		glNormal3f(1.f,0.f,0.f);
		glTexCoord2fv(t0);			glVertex3fv(c1);
		glTexCoord2fv(t1);			glVertex3fv(c5);
		glTexCoord2fv(t2);			glVertex3fv(c6);
		
		glTexCoord2fv(t0);			glVertex3fv(c1);
		glTexCoord2fv(t2);			glVertex3fv(c6);
		glTexCoord2fv(t3);			glVertex3fv(c2);
	//-X
		glNormal3f(-1.f,0.f,0.f);
		glTexCoord2fv(t3);			glVertex3fv(c7);
		glTexCoord2fv(t0);			glVertex3fv(c4);
		glTexCoord2fv(t1);			glVertex3fv(c0);
		
		glTexCoord2fv(t2);			glVertex3fv(c3);
		glTexCoord2fv(t3);			glVertex3fv(c7);
		glTexCoord2fv(t1);			glVertex3fv(c0);
	//+Y
		glNormal3f(0.f,1.f,0.f);
		glTexCoord2fv(t0);			glVertex3fv(c2);
		glTexCoord2fv(t1);			glVertex3fv(c6);
		glTexCoord2fv(t2);			glVertex3fv(c7);
		
		glTexCoord2fv(t0);			glVertex3fv(c2);
		glTexCoord2fv(t2);			glVertex3fv(c7);
		glTexCoord2fv(t3);			glVertex3fv(c3);
	//-Y
		glNormal3f(0.f,-1.f,0.f);
		glTexCoord2fv(t1);			glVertex3fv(c5);
		glTexCoord2fv(t2);			glVertex3fv(c1);
		glTexCoord2fv(t3);			glVertex3fv(c0);
		
		glTexCoord2fv(t0);			glVertex3fv(c4);
		glTexCoord2fv(t1);			glVertex3fv(c5);
		glTexCoord2fv(t3);			glVertex3fv(c0);

	glEnd();


	/*
	glBegin(GL_TRIANGLES);
	//+Z
		glNormal3f(0.f,0.f,1.f);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c0);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c1);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c2);
		
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c0);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c2);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c3);
	//-Z
		glNormal3f(0.f,0.f,-1.f);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c5);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c4);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c7);
		
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c5);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c7);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c6);
	//+X
		glNormal3f(1.f,0.f,0.f);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c1);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c5);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c6);
		
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c1);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c6);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c2);
	//-X
		glNormal3f(-1.f,0.f,0.f);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c7);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c4);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c0);
		
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c3);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c7);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c0);
	//+Y
		glNormal3f(0.f,1.f,0.f);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c2);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c6);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c7);
		
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c2);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c7);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c3);
	//-Y
		glNormal3f(0.f,-1.f,0.f);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c5);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t2);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t2);		glVertex3fv(c1);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c0);
		
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t0);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t0);		glVertex3fv(c4);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t1);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t1);		glVertex3fv(c5);
		glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, t3);		glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, t3);		glVertex3fv(c0);

	glEnd();
	*/

}

bool isInString(char *string, const char *search)
{
	int pos=0;
	int maxpos=strlen(search)-1;
	int len=strlen(string);	
	char *other;
	for (int i=0; i<len; i++) {
		if ((i==0) || ((i>1) && string[i-1]=='\n')) {				// New Extension Begins Here!
			other=&string[i];			
			pos=0;													// Begin New Search
			while (string[i]!='\n') {								// Search Whole Extension-String
				if (string[i]==search[pos]) pos++;					// Next Position
				if ((pos>maxpos) && string[i+1]=='\n') return true; // We Have A Winner!
				i++;
			}			
		}
	}	
	return false;													// Sorry, Not Found!
}

void InitTexture()
{
	glEnable(GL_TEXTURE_2D);


	bool result;
	result = LoadTGA(&textures[0], "box_tga.tga");
	result = LoadTGA(&textures[1], "l3d_tga.tga");

	GLubyte str[1024];

	char *extensions;	
	extensions=strdup((char *) glGetString(GL_EXTENSIONS));			// Fetch Extension String
	int len=strlen(extensions);
	for (int i=0; i<len; i++)										// Separate It By Newline Instead Of Blank
		if (extensions[i]==' ') extensions[i]='\n';
	printf("%s",extensions);
	


	// TEXTURE-UNIT #0		
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0].texID);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);	
	// TEXTURE-UNIT #1:
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[1].texID);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);

	glActiveTexture(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, textures[0].texID);
	glActiveTexture(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, textures[1].texID);


}



void DrawPlane()
{
	glBegin(GL_TRIANGLES);
		glNormal3fv(n1);
		glVertex3fv(p1);
		glNormal3fv(n2);
		glVertex3fv(p2);
		glNormal3fv(n3);
		glVertex3fv(p3);
	glEnd();

	glBegin(GL_TRIANGLES);
		glNormal3fv(n3);
		glVertex3fv(p3);
		glNormal3fv(n4);
		glVertex3fv(p4);
		glNormal3fv(n1);
		glVertex3fv(p1);
	glEnd();

}

void DrawSphere(float rad, int numLatitude, int numLongitude)
{	//WONIL : CAD&Graphics_01_ex.02 Sphere

	//최소한 육면체 이상은 되어야 하므로 위도는 2개, 경도는 3개를 최소로 한다.
	if(numLatitude<2)	numLatitude=2;
	if(numLongitude<3)	numLongitude=3;

	//삼각함수를 쓰기 위해 Degree가 아닌 Radian을 쓰도록 한다.
	float angleStepLati		= PI/(float)numLatitude;		//위도는 180도, 즉 PI를 나눈다.
	float angleStepLong	= 2.f* PI/(float)numLongitude;	//경도는 360도, 즉 2*PI를 나눈다.


	float prjLen, prjLenNext;
	float p1[3],p2[3],p3[3],p4[3];
	float n1[3],n2[3],n3[3],n4[3];
	//위도
	for(int countLati=0;countLati<numLatitude;countLati++)
	{
		p1[1] = p2[1] = rad * cos(angleStepLati * (float)countLati);			//기준 Y좌표.(위도)
		p3[1] = p4[1] = rad * cos(angleStepLati * (float)(countLati+1));		//다음 Y좌표.(위도)

		prjLen		= rad * sin(angleStepLati * (float)countLati);		//기준점의 XZ평면에 투영된 길이
		prjLenNext	= rad * sin(angleStepLati * (float)(countLati+1));	//다음점의 XZ평면에 투영된 길이

		for(int countLong=0;countLong<numLongitude;countLong++)
		{
			p1[0]	= prjLen * cos(angleStepLong * (float)countLong);	
			p1[2]	= prjLen * sin(angleStepLong * (float)countLong);	

			p2[0]	= prjLen * cos(angleStepLong * (float)(countLong+1));
			p2[2]	= prjLen * sin(angleStepLong * (float)(countLong+1));

			p3[0]	= prjLenNext * cos(angleStepLong * (float)countLong);	
			p3[2]	= prjLenNext * sin(angleStepLong * (float)countLong);	

			p4[0]	= prjLenNext * cos(angleStepLong * (float)(countLong+1));
			p4[2]	= prjLenNext * sin(angleStepLong * (float)(countLong+1));


//			glBegin(GL_LINE_LOOP);
			glBegin(GL_TRIANGLES);
				glNormal3fv(p1);
				glVertex3fv(p1);
				glNormal3fv(p3);
				glVertex3fv(p3);
				glNormal3fv(p2);
				glVertex3fv(p2);
			glEnd();

//			glBegin(GL_LINE_LOOP);
			glBegin(GL_TRIANGLES);
				glNormal3fv(p2);
				glVertex3fv(p2);
				glNormal3fv(p3);
				glVertex3fv(p3);
				glNormal3fv(p4);
				glVertex3fv(p4);
			glEnd();

		}


	}

}


 void LightInit()
{
    // Specify Black as the clear color
	glClearColor(0.0f,.0f,.0f,0.0f);
     // Specify the back of the buffer as clear depth
    glClearDepth(1.0f);
    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);  
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glFrontFace(GL_CCW);

	//Light
	//float pos[4] = {	1.f, 1.f, 1.f, 0.f	};	//Directional light
	float amb[4] = {	0.2f, 0.2f, 0.2f, 1.f	};
	float dif[4] = {	1.f, 1.f, 1.f, 1.f	};
	float spc[4] = {	1.f, 1.f, 1.f, 1.f	};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
//	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 25.0);
//	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,slDir);

	glEnable(GL_LIGHT0);

	glMaterialfv(GL_FRONT, GL_SPECULAR, spc);
	glMateriali(GL_FRONT, GL_SHININESS, 128);


//	glShadeModel(GL_FLAT);
}


GLint loc;
GLint tex,l3d;
float angle = 0.f;
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective.
	glOrtho(-1.f,1.f,-1.f,1.f,-1.f,1.f);
	//gluPerspective(45,ratio,1,100);
	glMatrixMode(GL_MODELVIEW);
}


#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
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
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void DrawOrientedCube()
{

	glPushMatrix();
	glRotatef(angle,0.f,1.f,0.f);
	glRotatef(30.f,1.f,1.f,1.f);
	//glutSolidCube(1.0);
	DrawCube();
	glPopMatrix();
}
void TurnObject()
{
	angle = angle + 1.f;
	if(angle>360.f)	angle-=360.f;
}
void BoundLight()
{
	if(bIncrease)
	{
		lpos[1] = lpos[1] + 0.02f;
		if(lpos[1]>=1.f)
		{
			lpos[1]=1.f;	bIncrease=false;
		}
	}
	else
	{
		lpos[1] = lpos[1] - 0.02f;
		if(lpos[1]<=-1.f)
		{
			lpos[1]=-1.f;	bIncrease=true;
		}
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

}

//Rendering function
void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//	glColor3f(.5f,0.5f,1.f);

	glColor3f(1.f,1.f,1.f);
	glPushMatrix();
	//glRotatef(angle,0.f,1.f,0.f);
	//glutWireTeapot(0.5);
	//glutSolidTeapot(0.5);
	//glBindTexture(GL_TEXTURE_2D, textures[0].texID);				// Bind Our Texture
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0].texID);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);	
	// TEXTURE-UNIT #1:
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[1].texID);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);

	DrawOrientedCube();

	/*
	glTranslatef(-.5f,0.f,0.f);
	DrawSphere(0.5f,10,10);
	glTranslatef(1.f,0.f,0.f);
	DrawSphere(0.5f,10,10);
	*/
	//DrawPlane();

	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	TurnObject();
}

GLuint v,f,f2,p;
void setShaders()
{

	char *vs = NULL,*fs = NULL,*fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	f2 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("minimal.vert");
	fs = textFileRead("minimal.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);

	free(vs);
	free(fs);

	glCompileShader(v);
	glCompileShader(f);

	printShaderInfoLog(v);
	printShaderInfoLog(f);
	printShaderInfoLog(f2);

	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);

	glLinkProgram(p);
	printProgramInfoLog(p);

	glUseProgram(p);

	loc = glGetUniformLocationARB(p,"lightDir");
	glUniform3fARB(loc, lpos[0],lpos[1],lpos[2]);


	loc=glGetUniformLocation(p, "tex"); 
	glUniform1i(loc, 0);
	loc=glGetUniformLocation(p, "l3d"); 
	glUniform1i(loc, 1);

}
void main(int argc, char **argv)
{
	//Initialize
	glutInit(&argc, argv); 
	//Set Display mode
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	//Set position
	glutInitWindowPosition(100,100);
	//Set size
	glutInitWindowSize(300,300);
	//Create Window
	glutCreateWindow("GLSL Base");


	//Set Display function
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);



	//Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	  exit(0);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	//OpenGL 2.0
	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(0);
	}

	LightInit();
	InitTexture();
	glEnable(GL_DEPTH_TEST);  

	//Set Shader
	setShaders();


	//Start Loop
	glutMainLoop();
}

