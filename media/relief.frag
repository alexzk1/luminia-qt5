
uniform sampler2D Depthmap;
uniform sampler2D Colormap;
uniform sampler2D Normalmap;
varying vec3 T,B,N;

varying vec3 EyeVec;

vec3 lvec= vec3(0.0,0,1); 

void main(void){
	vec3 eye = normalize(EyeVec);
	eye.x= -eye.x;
	
	vec2 rcoords = vec2(gl_TexCoord[0]);
	float i;
	for (i = 1.0; i>0;i -= 0.1){  
		float height = texture2D(Depthmap,rcoords ).r ;
		if (height > i)break;
		rcoords += eye.xy * 0.002;
		}

	

//*/
	//vec2 rcoords = vec2(gl_TexCoord[0]) + (eye.xy * height * 0.1-0.1);
	
	vec3 nmap = -1 + 2* texture2D(Normalmap,rcoords );
	nmap *= vec3(-1,-1,1);
	//nmap = vec3(0,0,1);
	//nmap = normalize(vec3(dot(T,nmap),dot(B,nmap), dot(N,nmap)));
        nmap = normalize(-T * nmap.x + B * nmap.y + N * nmap.z);
	float l = dot(nmap,lvec); 
        l += 1 * pow(max(dot(reflect (lvec,nmap),vec3(0,0,-1)), 0),4); 
	gl_FragColor = l * texture2D(Colormap,rcoords);
	//gl_FragColor = vec4(.5 + .5 * nmap,1);
	}