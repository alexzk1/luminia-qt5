attribute vec3 Tangent;

varying vec3 T,B,N;
varying vec3 EyeVec;

void main(void){
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 	gl_TexCoord[0]  = gl_MultiTexCoord0 * vec4(2,1,1,1);
	N = gl_Normal; 
	T = Tangent;
	B = cross (T,N);
	
	vec3 vm = vec3(gl_ModelViewMatrix * -gl_Vertex);

	EyeVec = vec3(dot(T,vm), dot(B,vm), dot(N,vm));

	}