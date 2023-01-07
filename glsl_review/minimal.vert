// minimal vertex shader
// www.lighthouse3d.com
void main()
{	
	//vec4 invColor;
	//invColor[0]=1.f-gl_Color[0];
	//invColor[1]=1.f-gl_Color[1];
	//invColor[2]=1.f-gl_Color[2];
	//invColor[3]=1.f-gl_Color[3];
	//gl_FrontColor = invColor;

	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}


