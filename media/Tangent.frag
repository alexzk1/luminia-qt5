 
varying vec3 col;
uniform sampler2D Tex0;
//uniform vec4 col;
void main(void){
 gl_FragColor = texture2D(Tex0, vec2(gl_TexCoord[0]));

gl_FragColor = vec4 (col *.5 +.5,0);
}