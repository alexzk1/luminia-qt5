/*
2018_REVISED
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

//if that used from "plugin" folder - that will be lost
brdfTool_functext =	"vec4 f(vec3 v){\n" +
		"   vec4 r;\n" +
		"   r[0] = 1.0;\n" +
		"   r[1] = 1.0;\n" +
		"   r[2] = 1.0;\n" +
		"   r[3] = 1.0;\n" +
		"   return r;}\n" ;

function brdfToolDialog(){
		
	var forcombo = new ComboBox();
	forcombo.itemList = Array("RGBA8","RGBA16F","RGBA32F");
	forcombo.enabled = false;
	
	var rescombo = new ComboBox();	
	rescombo.label = "Resolution:";
	rescombo.itemList = Array("no change", "16","32","64","128","256","512","1024");
	
	rescombo.changed.connect(function(){
		forcombo.enabled = !(rescombo.currentItem === "no change");
	});
	
	var func = new TextEdit();
	func.text = brdfTool_functext;
	
	var dialog = new Dialog();
	dialog.add(rescombo);
	dialog.add(forcombo);	
	dialog.add(func);

	if(dialog.exec()){		
		if(rescombo.currentItem != "no change"){
			obj.Image2d(rescombo.currentItem, rescombo.currentItem,obj.formatFromString(forcombo.currentItem));
		}
		print(obj.formatFromString(forcombo.currentItem));

		brdfToolFill(func.text);
		brdfTool_functext = func.text;
}


}

function brdfToolFill(f) {
	var vert =  "void main(void){ \n"+
				"gl_Position =  gl_Vertex * 2.0 - 1.0; \n" +
				"gl_TexCoord[0]  = gl_Position; \n" +
				"}";

	var frag =  "\n" +
				"void main(void){ \n" +
				"  vec2 i = gl_TexCoord[0].xy;\n" +
				"  vec3 v = normalize(vec3(i.xy, 1 - min(dot(i,i),1.0)));\n" +
				"  gl_FragColor = f(v);\n" +
				"  }";

	
	var fr = f + frag
	var Shader = gl.Shader(vert, fr);
	
	if (!Shader.isErrored()) {
		Shader.Bind();
		obj.BindFBO();
		gl.Begin(gl.QUADS);
			gl.Vertex(1,1);
			gl.Vertex(1,0);
			gl.Vertex(0,0);
			gl.Vertex(0,1);
		gl.End();
		obj.UnbindFBO();
	}else {
		print('Shader error in function brdfToolFill(f)');
		print('Errored Vert:\n', vert);
		print('Errored Frag:\n', fr);
	}
}





