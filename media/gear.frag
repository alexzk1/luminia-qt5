varying vec3 T,B,N;
uniform sampler2D Tex0;
uniform vec4 col;
void main(void){
	vec3 lv =  vec3 (0.0 ,0.707,0.707);
	T = normalize (T);
	vec3 n = normalize(lv - T * dot(lv,T));
	n= normalize(n +N);

	float r = pow(max(dot(reflect (lv,n),vec3(0.0,0,-1)), 0),8);

	gl_FragColor = col *dot(normalize(n),lv) + r; 
	}
