const float pi = 3.14159265 ;

vec4 rand(vec2 A,vec2 B,vec2 C,vec2 D){
	vec4 tmp = vec4( dot(fract(A),vec2(pi ,pi*56.67)),dot(fract(B),vec2(pi ,pi*56.67)),dot(fract(C),vec2(pi ,pi*56.67)),dot(fract(D),vec2(pi ,pi*56.67)));
	return fract(tan(tmp)  * 43758.5453);
	}

float rands(vec2 coord,float d){
	vec2 A = floor( coord * d)/d +d;
	vec2 B = A + vec2(1/d,0);
	vec2 C = A + vec2(1/d,1/d);
	vec2 D = A + vec2(0,1/d);
	
	vec4 delta = vec4(fract(coord.xy * d),0,0);
	delta.zw = -delta.xy + 1.0;
	vec4 w = vec4(delta.x * delta.y, delta.z * delta.y, delta.z * delta.w, delta.x * delta.w);
	vec4 f = w * w / dot(w,w);
	w = (w+f) /2.0;
	return dot(vec4(rand(C,D,A,B)),w);
	}

void main(void){
	vec4 a = vec4( rands(gl_TexCoord[0].xy,16.0), rands(gl_TexCoord[0].xy,32.0), rands(gl_TexCoord[0].xy, 64.0),rands(gl_TexCoord[0].xy, 128.0));
	float r = dot(a,vec4(0.4,0.3,0.2,0.1));
	gl_FragColor = vec4 (r,r,r,1);
	}
