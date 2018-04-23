/* <XPM>
<NAME>MandelBrot</NAME>
<FILTER>Texture</FILTER>
<DESCRIPTION>Fill a texture with an Appleman</DESCRIPTION>

static char *mandelbrot[]={
"16 16 14 1",
". c #000000",
"# c #000040",
"a c #000080",
"b c #0000c0",
"c c #0058c0",
"l c #010785",
"k c #02108c",
"i c #0250c5",
"e c #030204",
"f c #030306",
"h c #040304",
"g c #050306",
"d c #054ec1",
"j c #064dc1",
"..#abbbbbbbbbaaa",
"..aabbbbbbbbbbaa",
".aabbbbbbcccbbba",
".abbbbbbccacccbb",
"#abbbbbcc#..ccdb",
"abbbcccc#....ccb",
"abbbc#cc......cb",
"accc#ef#.....bcb",
"accc#gh#.....bcb",
"abbbc#cc......cb",
"abbbbccc#....ccb",
"#abbbbicc#..ccjb",
".abbbbbbccacccbb",
".aabbbbbbccccbba",
"..aabbbbbbbbbbaa",
"..#akbbbbbbbblaa"};
</XPM> */


v= "void main(void){gl_Position = gl_Vertex * 2.0 - 1.0 ;gl_TexCoord[0] = gl_MultiTexCoord0;}";
f="void main (){vec2 c = gl_TexCoord[0].xy; vec2 z = c;float i;for ( i = 0.0; i < 1.0; i += 1.0/256.0){          z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c; if (dot(z, z) > 4.0) {  break;}}gl_FragColor = vec4(i,i,i,1.0);}";

//Texture = World.getSelected().addTexture("Texture");

d = new Dialog;
d.caption = "Mandelbrot Creator";
d.okButtonText = "Create";
d.cancelButtonText = "Cancel";

spin = new SpinBox;
spin.label = "Resolution: 2 power";
spin.minimum = 4;
spin.maximum = 12;
spin.value = 10;
d.add(spin);
/*
check = new CheckBox;
check.label = "Save Image";
d.add( check );
*/
g = new GroupBox;
g.title = "Coords:";
d.add( g );

x1 = new NumberEdit;
x2 = new NumberEdit;
y1 = new NumberEdit;
y2 = new NumberEdit;

x1.label = "Start X";
x2.label = "Stop  X";
y1.label = "Start Y";
y2.label = "Stop  Y";

x1.value = -1.0;
y1.value = -1.0;
x2.value =  1.0;
y2.value =  1.0;

g.add(x1);
g.add(x2);
g.add(y1);
g.add(y2);

if (d.exec()){
	World.addNode("Temp");
	World.Temp.addScript();
	gl = World.Temp.Script.gl;

	Texture = World.getContext();
	res = Math.pow(2,spin.value);
	Texture.Image2d(res,res,gl.RGBA);

	Texture.BindFBO();
	shader = gl.Shader(v,f);
	shader.Bind();
	gl.Begin(gl.QUADS);
		gl.TexCoord(x1.value,y1.value);
		gl.Vertex(1,1);
		gl.TexCoord(x1.value,y2.value);
		gl.Vertex(1,0);
		gl.TexCoord(x2.value,y2.value);
		gl.Vertex(0,0);
		gl.TexCoord(x2.value,y1.value);
		gl.Vertex(0,1);
	gl.End();
	shader.Unbind();
	Texture.UnbindFBO();

	World.Temp.deleteLater();
	}










