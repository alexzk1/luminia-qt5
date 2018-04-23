/*
<FILTER>Texture</FILTER>
<FUNCTION>brdfToolDialog()</FUNCTION>

<ACTION>
<SLOT>brdfToolDialog()</SLOT>
<TEXT>Create BRDF Factors</TEXT>
<XPM>
static char *gear[]={
"16 16 5 1",
". c None",
"# c #000000",
"a c #c00000",
"b c #ff0000",
"c c #ffc0c0",
"................",
".##############.",
".##############.",
".##.............",
".##.............",
".##.............",
".##.............",
".#########......",
".#########......",
".##.............",
".##.............",
".##.............",
".##.............",
".##.............",
".##.............",
"................"};
</XPM>
</ACTION>

*/

//dialog = 0;
/*
vert = "void main(void){ \n"+
	"gl_Position =  gl_Vertex * 2.0 - 1.0; \n" +
	"gl_TexCoord[0]  = gl_Vertex; \n" +
	"}";


frag=	"uniform sampler2D Src;\n" +
	"uniform vec4 r,g,b;\n" +
	"uniform float lum;\n" +
	"uniform float alpha;\n" +
	"void main(void){\n" +
	"vec4 col = texture2D(Src, gl_TexCoord[0].xy);\n" +
	"col = mix(col, col * col.a, alpha);\n" +
	"gl_FragColor.r = lum * dot(r,col);\n"+
	"gl_FragColor.g = lum * dot(g,col);\n"+
	"gl_FragColor.b = lum * dot(b,col);\n"+
	"gl_FragColor.a = 1.0;\n"+
	"}\n";


Shader = gl.Shader(vert,frag);
Shader.Uniform("r",1,0,0,0);
Shader.Uniform("g",0,1,0,0);
Shader.Uniform("b",0,0,1,0);
Shader.Uniform("lum", 1.0);
Shader.Uniform("alpha", 0.0);
*/

functext =	"vec4 f(vec3 v){\n" +
		"   vec4 r;\n" +
		"   r[0] = 1.0;\n" +
		"   r[1] = 1.0;\n" +
		"   r[2] = 1.0;\n" +
		"   r[3] = 1.0;\n" +
		"   return r;}\n" ;



function changed(){
	if(rescombo.currentItem == "no change"){
		forcombo.enabled = false;
		}
	else{
		forcombo.enabled = true;
		}
	print("changed");
	}

function brdfToolDialog(){
	print ("context menu starts script function");

	dialog = new Dialog();

	rescombo = new ComboBox();
	rescombo.itemList = Array("no change", "16","32","64","128","256","512","1024");
	rescombo.label = "Resolution:";
	rescombo.changed.connect(changed);
	dialog.add(rescombo);

	forcombo = new ComboBox();
	forcombo.itemList = Array("RGBA8","RGBA16F","RGBA32F");
	rescombo.label = "Format:";
	forcombo.enabled = false;
	dialog.add(forcombo);

	

	func = new TextEdit();
	func.text = functext;

	dialog.add(func);

	if(dialog.exec()){
	
		if(rescombo.currentItem != "no change"){
			obj.Image2d(rescombo.currentItem, rescombo.currentItem,obj.formatFromString(forcombo.currentItem));
			}
		print(obj.formatFromString(forcombo.currentItem));

		brdfToolFill(func.text);
		functext = func.text;

		}


	}

vert =  "void main(void){ \n"+
	"gl_Position =  gl_Vertex * 2.0 - 1.0; \n" +
	"gl_TexCoord[0]  = gl_Position; \n" +
	"}";

frag =  "\n" +
	"void main(void){ \n" +
	"  vec2 i = gl_TexCoord[0].xy;\n" +
	"  vec3 v = normalize(vec3(i.xy, 1 - min(dot(i,i),1.0)));\n" +
	"  gl_FragColor = f(v);\n" +
	"  }";






function brdfToolFill(f){

	Shader = new gl.Shader(vert, f +frag);
	Shader.Bind();

	print(f +frag);


	obj.BindFBO();
	gl.Begin(gl.QUADS);
		gl.Vertex(1,1);
		gl.Vertex(1,0);
		gl.Vertex(0,0);
		gl.Vertex(0,1);
	gl.End();

	obj.UnbindFBO();
	}





