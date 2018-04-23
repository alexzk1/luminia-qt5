/*
<FILTER>Texture</FILTER>
<FUNCTION>textureTool()</FUNCTION>

<ACTION>
<SLOT>textureTool()</SLOT>
<TEXT>Show texture</TEXT>
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
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"......##........",
"................"};
</XPM>
</ACTION>

*/

dock = 0;

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

function close(){
	print("close called");
	dock.deleteLater(); // delete the dialog and widgets
	dock = 0;
	}




function changed() {

	switch(combo.currentItem){
		case "RGB":
			Shader.Uniform("r",1,0,0,0);
			Shader.Uniform("g",0,1,0,0);
			Shader.Uniform("b",0,0,1,0);
			break;
		case "R":
			Shader.Uniform("r",1,0,0,0);
			Shader.Uniform("g",1,0,0,0);
			Shader.Uniform("b",1,0,0,0);
			break;
		case "G":
			Shader.Uniform("r",0,1,0,0);
			Shader.Uniform("g",0,1,0,0);
			Shader.Uniform("b",0,1,0,0);
			break;
		case "B":
			Shader.Uniform("r",0,0,1,0);
			Shader.Uniform("g",0,0,1,0);
			Shader.Uniform("b",0,0,1,0);
			break;
		case "A":
			Shader.Uniform("r",0,0,0,1);
			Shader.Uniform("g",0,0,0,1);
			Shader.Uniform("b",0,0,0,1);
			break;
		}

	if(alphacheck.checked)Shader.Uniform("alpha", 1.0);
	else Shader.Uniform("alpha", 0.0);

	print (lumslider.value);
	Shader.Uniform("lum", Math.pow(Math.sqrt(2), lumslider.value/2));

	glw.update();
	}


function paintGL(){
	//gl.Clear();
	print("paintGL event");

	obj.Bind(0);
	Shader.Bind();
	gl.Begin(gl.QUADS);
		gl.Vertex(1,1);
		gl.Vertex(1,0);
		gl.Vertex(0,0);
		gl.Vertex(0,1);
	gl.End();
	Shader.Unbind();

	}

function resizeGL(){
	print ("resizeGL event");
	}


function textureTool(){
	print ("context menu starts script function");

	if (dock == 0){
		dock = new Dock();
		dock.title = obj.objectName;

		glw = new GLWidget();
		glw.paintGL.connect(paintGL);
		glw.resizeGL.connect(resizeGL);
		dock.add(glw);
	
		dock.newColumn();
		dock.newTab("General");

		cb = new Button();
		cb.text = "Close" ;
		cb.clicked.connect(close);
		dock.add(cb);

		lb = new Button();
		lb.text = "Load" ;
		lb.clicked.connect(obj.load);
		dock.add(lb);


		btb = new Button();
		btb.text = "BRDF factor";
		btb.clicked.connect(obj.brdfToolDialog);
		dock.add(btb);



		dock.newTab("View");

		combo = new ComboBox();
		combo.itemList = Array("RGB","R","G","B","A");
		combo.label = "";
		combo.changed.connect(changed);
		dock.add(combo);

		lumslider = new Slider()
		lumslider.minimum = -10;
		lumslider.maximum = 10;
		lumslider.value = 0;
		lumslider.changed.connect(changed);
		lumslider.label = "Lum";
		dock.add(lumslider);

		alphacheck = new CheckBox();

		alphacheck.text = "RGB * A";
		alphacheck.checked = false;
		alphacheck.clicked.connect(changed);
		dock.add(alphacheck);

		dock.show();
		}
	}





