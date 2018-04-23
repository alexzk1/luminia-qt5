uniform sampler2D Tex0; 
varying vec3 T,B,N; 
 
vec3 lvec= vec3(0.707,0,0.707); 
 
 
void main(void){ 
	vec3 nmap = -1 + 2* texture2D(Tex0, vec2(gl_TexCoord[0])); 
	nmap = normalize(vec3(dot(T,nmap),dot(B,nmap), dot(N,nmap))); 
	//gl_FragColor = vec4 (nmap *.5 +.5,0); 
	float l = dot(nmap,lvec); 
	 l += 2 * pow(max(dot(reflect (lvec,nmap),vec3(0,0,-1)), 0),4); 
	gl_FragColor =l * texture2D(Tex0, vec2(gl_TexCoord[0])+vec2(0,.5)); 
	} 