// minimal vertex shader
// www.lighthouse3d.com
void main()
{	
	gl_FrontColor = gl_Color;
	vec4 newPos = vec4(gl_Vertex);
	newPos.y = newPos.y + 0.5;
	gl_Position = gl_ModelViewProjectionMatrix * newPos;
}


