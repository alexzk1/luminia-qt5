/*
<FILTER>Texture</FILTER>
<FUNCTION>seperateToolDialog()</FUNCTION>

<ACTION>
<SLOT>seperateToolDialog()</SLOT>
<TEXT>Seperate Texture</TEXT>
<XPM>
static char *gear[]={
"16 16 5 1",
". c None",
"# c #000000",
"a c #c00000",
"b c #ff0000",
"c c #ffc0c0",
"................",
".....##########.",
"..#############.",
".###............",
".##.............",
".##.............",
".###............",
"..##########....",
"....#########...",
"...........###..",
"............##..",
"............##..",
"............##..",
"..############..",
"..##########....",
"................"};
</XPM>
</ACTION>
*/




function changed(){
	if(rescombo.currentItem == "no change"){
		forcombo.enabled = false;
		}
	else{
		forcombo.enabled = true;
		}
	print("changed");
	}

function seperateToolDialog(){
	print ("context menu starts script function");

	dialog = new Dialog();

	c = Array(4);

	for(i = 0; i< 4; i++){
		dialog.newTab("Color" + i);

		c[i] = new ColorWidget();
		dialog.add(c[i]);
		}

	if(dialog.exec()){
	
		seperateToolFill(c[0].color, c[1].color, c[2].color, c[3].color);

		}


	}

vert =  "void main(void){ \n"+
	"gl_Position =  gl_Vertex * 2.0 - 1.0; \n" +
	"gl_TexCoord[0]  = gl_Vertex; \n" +
	"}";

frag =  "uniform vec3 c0;\n" +
	"uniform vec3 c1;\n" +
	"uniform vec3 c2;\n" +
	"uniform vec3 c3;\n" +
	"uniform sampler2D tex;\n" +

	"void main(void){ \n" +
	"  vec3 col =  texture2D(tex, gl_TexCoord[0].xy).rgb;\n" +
	"  vec4 l;\n" +
	"  l.x = 1.0 - length(col-c0);\n" +
	"  l.y = 1.0 - length(col-c1);\n" +
	"  l.z = 1.0 - length(col-c2);\n" +
	"  l.w = 1.0 - length(col-c3);\n" +
	"  l -= min(min(l.x,l.y),min(l.z,l.w));\n" +

	"  l = pow(l, 4.0);\n" +

	"  l /= dot(l,vec4(1.0, 1.0, 1.0, 1.0));\n" + 

	"  gl_FragData[0].rgb = col * l[0];\n" +
	"  gl_FragData[1].rgb = col * l[1];\n" +
	"  gl_FragData[2].rgb = col * l[2];\n" +
	"  gl_FragData[3].rgb = col * l[3];\n" +
	"  }";






function seperateToolFill(c0,c1,c2,c3){


	par = obj.getParent();

	W = obj.Width();
	H = obj.Height();
	
	tex1 = par.addTexture("Out1");
	tex2 = par.addTexture("Out2");
	tex3 = par.addTexture("Out3");
	tex4 = par.addTexture("Out4");
	
	tex1.Image2d(W,H,gl.RGBA8);
	tex2.Image2d(W,H,gl.RGBA8);
	tex3.Image2d(W,H,gl.RGBA8);
	tex4.Image2d(W,H,gl.RGBA8);



	framebuffer = gl.Framebuffer();
	framebuffer.Append(tex1,0);
	framebuffer.Append(tex2,1);
	framebuffer.Append(tex3,2);
	framebuffer.Append(tex4,3);


	Shader = new gl.Shader(vert, frag);
	Shader.Bind();

	Shader.Uniform("c0",c0);
	Shader.Uniform("c1",c1);
	Shader.Uniform("c2",c2);
	Shader.Uniform("c3",c3);

	print(frag);


	framebuffer.Bind();

	gl.Begin(gl.QUADS);
		gl.Vertex(1,1);
		gl.Vertex(1,0);
		gl.Vertex(0,0);
		gl.Vertex(0,1);
	gl.End();

	framebuffer.Unbind();
	}





