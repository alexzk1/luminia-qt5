
uniform sampler2D Tex0;
varying vec3 T,B,N;
void main(void){
vec3 nmap = -1 + 2* texture2D(Tex0, vec2(gl_TexCoord[0]));
nmap = normalize(vec3(dot(T,nmap),dot(B,nmap), dot(N,nmap)));
//gl_FragColor = vec4 (nmap *.5 +.5,0);
gl_FragColor = dot(nmap,vec3(0,0,1));
}