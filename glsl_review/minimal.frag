varying vec3 lightDir,normal;
uniform sampler2D tex,l3d;

void main()
{
	vec3 ct,cf,c;
	vec4 texel;
	float intensity,at,af,a;
	
	intensity = max(dot(lightDir,normalize(normal)),0.0);
	
	cf = intensity * (gl_FrontMaterial.diffuse).rgb + 
					  gl_FrontMaterial.ambient.rgb;
	af = gl_FrontMaterial.diffuse.a;
	
	texel = texture2D(tex,gl_TexCoord[0].st);
		
	ct = texel.rgb;
	at = texel.a;
	
	c = cf * ct;
	a = af * at;
		
	float coef = smoothstep(1.0,0.2,intensity);
	//c += coef *  vec3(texture2D(l3d,gl_TexCoord[0].st));
	c += vec3(texture2D(l3d,gl_TexCoord[1].st));
	
	gl_FragColor = vec4(c, a);	
}
