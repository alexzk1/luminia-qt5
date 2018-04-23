
varying vec3 T,B,N;
attribute vec3 Tangent;
varying vec3 col;
void main(void){
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 	gl_TexCoord[0]  = gl_MultiTexCoord0;
	N = gl_Normal; 
	T = Tangent;
	B = cross (T,N);
	}