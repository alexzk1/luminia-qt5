/*
<NAME>2D Noise</NAME>
<FILTER>Texture</FILTER>
<DESCRIPTION>Creates a 3d noise texture</DESCRIPTION>
<XPM>
static char * noise_xpm[] = {
"16 16 8 1",
"       c #070905",
".      c #2E2E2C",
"+      c #484846",
"@      c #676764",
"#      c #858482",
"$      c #A9A8A6",
"%      c #C9C8C6",
"&      c #F7F5F3",
"..@#####@+@#@+@#",
"..@####@@+@#@+@#",
"@@####@....+..@#",
"######+       @$",
"++++@@.     .#$$",
"    .@+...  @%&%",
"    .@$$+ .#$%$$",
"    .#%&# @&%#@#",
"++. +#$%$#%&$+@#",
"##+.+###%&&%#+@#",
"@+. @$%%$##$$#@@",
"    @%&&# .#$%#+",
"++++@#$$@..+#$@+",
"####+.+@@@@+@@@+",
"##@+++@##$#@@@..",
"##. +$$$$$$$#@. "};
</XPM>
*/



vert = "void main(void){ \n gl_Position =  gl_Vertex * 2.0 - 1.0; \n gl_TexCoord[0]  = gl_Vertex; \n }";

d = new Dialog;
d.caption = "Noise";
d.okButtonText = "Create";
d.cancelButtonText = "Cancel";

spin = new SpinBox;
spin.label = "Resolution: 2 power";
spin.minimum = 2;
spin.maximum = 16;
spin.value = 5;
d.add(spin);


if (d.exec()){

	Con = World.getContext();
	Node = Con.getParent();
	
	Out = Node.addTexture("Noise");
	
	Out.Image2d(Math.pow(2,spin.value),Math.pow(2,spin.value),gl.RGBA);
	frag = "uniform float slice;" +
		" float rand(vec2 co){" +
		" return fract(sin(dot(vec3(co.xy,slice) ,vec3(12.9898,78.233, 34.897))) * 43758.5453); " +
		" }" + 
		"void main(void){ " +
		" gl_FragColor = rand(gl_TexCoord[0].xy);" + 
		" }";

        print(vert);

	Shader = gl.Shader(vert,frag);
	
	Out.BindFBO();
	Con.Bind(0);
	Shader.Bind();

	gl.Begin(gl.QUADS);
		gl.Vertex(1,1);
		gl.Vertex(1,0);
		gl.Vertex(0,0);
		gl.Vertex(0,1);
	gl.End();

	Shader.Unbind();
	Con.Unbind();
	Out.UnbindFBO();
	}






