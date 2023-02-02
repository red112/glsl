uniform vec3 lightDir;
void main()
{
	float intensity;
	intensity = dot(lightDir,gl_Normal);
	vec4 color;

	color[0] = gl_Color[0]*intensity;	
	color[1] = gl_Color[1]*intensity;	
	color[2] = gl_Color[2]*intensity;	
	color[3] = 1.0;
		
	gl_FrontColor  = color;
	
	gl_Position = ftransform();
} 



