
uniform sampler2D Depthmap;
uniform sampler2D Colormap;
uniform sampler2D Normalmap;
varying vec3 T,B,N;

varying vec3 EyeVec;

void main(void){
	vec3 eye = normalize(EyeVec);

	
	float height = texture2D(Depthmap,vec2(gl_TexCoord[0]) ).r ;
	eye.xy= -eye.xy;
	vec2 rcoords = vec2(gl_TexCoord[0]) + (eye.xy * height * 0.1);


	vec3 nmap = -1 + 2* texture2D(Normalmap,rcoords );
	nmap = normalize(vec3(dot(T,nmap),dot(B,nmap), dot(N,nmap)));

	gl_FragColor = dot(nmap,vec3(0,0,1)) * texture2D(Colormap,rcoords);
}