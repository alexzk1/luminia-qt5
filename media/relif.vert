attribute vec3 Tangent;
varying vec3 T,B,N;
varying vec3 EyeVec;

void main(void){
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 	gl_TexCoord[0] = gl_MultiTexCoord0 * vec4(3,1,1,1);
	N = normalize(gl_NormalMatrix * gl_Normal); 
	T = normalize(gl_NormalMatrix  * Tangent);
	B = cross (T,N);
	
	vec3 vm = vec3(gl_ModelViewMatrix * -gl_Vertex);

	EyeVec = vec3(dot(T,vm), dot(B,vm), dot(N,vm));

	}