uniform vec3 lightDir;
varying vec3 normal;

void main()
{
         vec4 color;
		 float intensity;
		 intensity = max(dot(lightDir,normalize(normal)),0.0);

         if (intensity > 0.95)        color = vec4(0.5,0.5,1.0,1.0);
         else if (intensity > 0.5)    color = vec4(0.3,0.3,0.6,1.0);
         else if (intensity > 0.25)   color = vec4(0.2,0.2,0.4,1.0);
         else                        color = vec4(0.1,0.1,0.2,1.0);        
         gl_FragColor = color;
}

