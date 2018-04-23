 
attribute vec3 Tangent;
varying vec3 col;
void main(void)
{
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 gl_TexCoord[0]  = gl_MultiTexCoord0;
col = Tangent;
}