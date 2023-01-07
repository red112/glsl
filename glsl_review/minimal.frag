// minimal fragment shader
// www.lighthouse3d.com
void main()
{
	vec4 invColor;
	invColor[0]=1.f-gl_Color[0];
	invColor[1]=1.f-gl_Color[1];
	invColor[2]=1.f-gl_Color[2];
	invColor[3]=1.f-gl_Color[3];

	//gl_FragColor = gl_Color;
	gl_FragColor = invColor;
}
