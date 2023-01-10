uniform float angle;
void main()
{	
	gl_FrontColor = gl_Color;
	vec4 newPos = vec4(gl_Vertex);
	newPos.y = newPos.y + sin(8.0*newPos.x+angle) / 4.0;
	gl_Position = gl_ModelViewProjectionMatrix * newPos;
}


