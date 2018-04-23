varying vec3 T,B,N;

const float pi = 3.14159265 ;

vec4 rand(vec4 A,vec4 B,vec4 C,vec4 D){
	vec4 s = vec4 (pi * 7.9557 ,pi*56.67,pi*6.612 , pi * 2.0);
	vec4 tmp = vec4( dot(A,s),dot(B,s),dot(C,s),dot(D,s));
	return fract(tan(tmp)  * 43758.5453);
	}

float noise(vec4 coord,float d){
	vec4 C[16];
	C[0] = floor( coord * d)/d +d;
	C[1] = C[0] + vec4(1/d,0,0,0);
	C[2] = C[0] + vec4(1/d,1/d,0,0);
	C[3] = C[0] + vec4(0,1/d,0,0);

	C[4] = C[0] + vec4(0,0,1/d,0);
	C[5] = C[0] + vec4(1/d,0,1/d,0);
	C[6] = C[0] + vec4(1/d,1/d,1/d,0);
	C[7] = C[0] + vec4(0,1/d,1/d,0);

	C[8] = C[0] + vec4(0,0,0,1/d);
	C[9] = C[0] + vec4(1/d,0,0,1/d);
	C[10] = C[0] + vec4(1/d,1/d,0,1/d);
	C[11] = C[0] + vec4(0,1/d,0,1/d);

	C[12] = C[0] + vec4(0,0,1/d,1/d);
	C[13] = C[0] + vec4(1/d,0,1/d,1/d);
	C[14] = C[0] + vec4(1/d,1/d,1/d,1/d);
	C[15] = C[0] + vec4(0,1/d,1/d,1/d);

	vec4 p = fract(coord * d);
	vec4 q = 1.0 - p;

	vec4 w = vec4(q.x * q.y, p.x * q.y, p.x * p.y, q.x * p.y);
	vec4 w0 = w * q.z * q.w;
	vec4 w1 = w * p.z * q.w;
	vec4 w2 = w * q.z * p.w;
	vec4 w3 = w * p.z * p.w;

	vec4 f0 = w0 * w0;
	vec4 f1 = w1 * w1;
	vec4 f2 = w2 * w2;
	vec4 f3 = w3 * w3;
	float l = dot(w0,w0) + dot(w1,w1)+ dot(w2,w2) + dot(w3,w4) ;
	 f0 /= l; 
	f1 /= l;
	 f2 /= l; 
	f3 /= l;

	w0 = (w0+f0) /2.0;
	w1 = (w1+f1) /2.0;
	w2 = (w2+f2) /2.0;
	w3 = (w3+f3) /2.0;


	return dot(vec4(rand(C[0],C[1],C[2],C[3])),w0) + dot(vec4(rand(C[4],C[5],C[6],C[7])),w1)  + 	   dot(vec4(rand(C[8],C[9],C[10],C[11])),w2) + dot(vec4(rand(C[12],C[13],C[14],C[15])),w3);
	}



void main(void){
vec4 a = vec4( rands(gl_TexCoord[0].xyz,16.0), rands(gl_TexCoord[0].xyz,32.0), rands(gl_TexCoord[0].xyz, 64.0),rands(gl_TexCoord[0].xyz, 128.0));
	float r = dot(a,vec4(0.5,0.3,0.3,0.6));
//*/
	r = abs(sin(r * 10)) * dot(N,vec3(0,0,1));
	gl_FragColor = vec4 (r,r,r,1);
	}