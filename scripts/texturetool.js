/*
2018_REVISED
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

texture_tool_dock = 0;
texture_tool_glw = 0;
texture_tool_lumslider = 0;
texture_tool_alphacheck = 0;
texture_tool_combo = 0;

texture_tool_vert = "void main(void){ \n"+
	"gl_Position =  gl_Vertex * 2.0 - 1.0; \n" +
	"gl_TexCoord[0]  = gl_Vertex; \n" +
	"}";


texture_tool_frag=	"uniform sampler2D Src;\n" +
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


texture_tool_Shader = gl.Shader(texture_tool_vert,texture_tool_frag);
texture_tool_Shader.Uniform("r",1,0,0,0);
texture_tool_Shader.Uniform("g",0,1,0,0);
texture_tool_Shader.Uniform("b",0,0,1,0);
texture_tool_Shader.Uniform("lum", 1.0);
texture_tool_Shader.Uniform("alpha", 0.0);

function texture_tool_paintGL(){
	//gl.Clear();
	//print("paintGL")
	var Shader = texture_tool_Shader;
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

function texture_tool_changed() {	
	var Shader = texture_tool_Shader;
	switch(texture_tool_combo.currentItem){
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

	if(texture_tool_alphacheck.checked) Shader.Uniform("alpha", 1.0);
	else                                Shader.Uniform("alpha", 0.0);
	Shader.Uniform("lum", Math.pow(Math.sqrt(2), texture_tool_lumslider.value/2));

	texture_tool_glw.update();	
}


function textureTool(){	
	if (texture_tool_dock == 0){
		texture_tool_dock = new Dock();
		texture_tool_dock.title = obj.objectName;

		texture_tool_glw = new GLWidget();
		texture_tool_glw.paintGL.connect(texture_tool_paintGL);		
		texture_tool_dock.add(texture_tool_glw);
	
		texture_tool_dock.newColumn();
		texture_tool_dock.newTab("General");

		var cb = new Button();
		cb.text = "Close" ;
		cb.clicked.connect(function(){
			texture_tool_Shader.deleteLater();
			texture_tool_Shader = 0;
			texture_tool_dock.deleteLater(); // delete the dialog and widgets
			texture_tool_dock = 0;
			texture_tool_glw  = 0;
		});
		texture_tool_dock.add(cb);

		var lb = new Button();
		lb.text = "Load" ;
		lb.clicked.connect(obj.load); //this is C++ slot
		texture_tool_dock.add(lb);


		var btb = new Button();
		btb.text = "BRDF factor";				
		btb.clicked.connect(obj, brdfToolDialog); //this is  engine variable now, so syntax differs
		
		texture_tool_dock.add(btb);



		texture_tool_dock.newTab("View");

		var combo = new ComboBox();
		texture_tool_combo = combo;
		combo.itemList = Array("RGB","R","G","B","A");
		combo.label = "";
		combo.changed.connect(texture_tool_changed);
		texture_tool_dock.add(combo);

		var lumslider = new Slider()
		texture_tool_lumslider = lumslider;
		
		lumslider.minimum = -10;
		lumslider.maximum = 10;
		lumslider.value = 0;
		lumslider.changed.connect(texture_tool_changed);
		lumslider.label = "Lum";
		texture_tool_dock.add(lumslider);

		var alphacheck = new CheckBox();
		texture_tool_alphacheck = alphacheck;
		alphacheck.text = "RGB * A";
		alphacheck.checked = false;
		alphacheck.clicked.connect(texture_tool_changed);
		texture_tool_dock.add(alphacheck);				
		}
	    texture_tool_dock.show();
}





