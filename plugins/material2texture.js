/*
<NAME>Material2Material</NAME>
<FILTER>Index</FILTER>
<DESCRIPTION>Renders a fragmentshaders output to a texture for converting procedural materials</DESCRIPTION>
<XPM>
static char *material[]={
"16 16 13 1",
". c #000000",
"k c #00ff00",
"c c #808080",
"a c #a0a0a0",
"g c #a8dcff",
"j c #c0c0ff",
"f c #c0ffc0",
"h c #c0ffff",
"d c #c3c3c3",
"b c #ffc0c0",
"e c #ffc0ff",
"# c #ffffc0",
"i c #ffffff",
"................",
"......###aaab...",
"...####aaacba...",
"...##decc#baafg.",
"..dggeffh#aafgg.",
".dggeffhbbafgg..",
".ggfffh#baaggi..",
".effh###ffdjki..",
".ffhh#hfddajka..",
".chhhahfdbj##a..",
".chhahhdbb##aa..",
"..haahbbb##aa...",
"..aahhbj##ia....",
"..ahbbj##ia.....",
"................",
"................"};
</XPM>
*/

v = "varying vec3 N;\n void main(void){ \n gl_Position =  gl_MultiTexCoord0 * 2.0 - 1.0; \n N = gl_Normal; \n gl_TexCoord[0] = gl_Vertex;\n}";

d = new Dialog;
d.caption = "Render Material to texture";
d.okButtonText = "Continue";
d.cancelButtonText = "Cancel";

spin = new SpinBox;
spin.label = "Resolution: 2 power";
spin.minimum = 4;
spin.maximum = 12;
spin.value = 10;
d.add(spin);

var g = new GroupBox;
g.title = "Use Fragmentshader:";
d.add( g );

var r_file = new RadioButton;
r_file.text = "File";
r_file.checked = true;
g.add(r_file);

var r_paste = new RadioButton;
r_paste.text = "Paste";
g.add(r_paste);

var save = new CheckBox;
save.text = "Save to png";
d.add(save);


if (d.exec()){
	if (r_file.checked){
		file = new File(FileDialog.getOpenFileName( "*.glsl *.GLSL *.frag *.FRAG"));
		file.open(File.ReadOnly);
		f = file.read();
		}
	else{
		f = Input.getText( "Paste shader" );
		if(!f) f="";
		}

	if (save.checked)
		fn = FileDialog.getSaveFileName( "*.png" );
	else 
		fn = "";

		


	Con = World.getContext();
	Mesh = Con.getParent();
	Node = Mesh.getParent();
if(Node.findChild("Colormap"))print("found colormap");


	Node.addTexture("Material");
	Material = Node.Material ;

	res = Math.pow(2,spin.value);

	Material.Image2d(res,res,gl.RGBA);

	Shader = gl.Shader(v,f);
	
	Material.BindFBO();
	
	Shader.Bind();


	reg = /sampler[1-3]D ([a-zA-Z0-9]+)/;

	source = f;

	for (i = 0;i < 4;i++){
		reg.search(source);
		res = reg.cap(1);
		if (!res) break;
		print (res);
		source = source.replace(reg.cap(0),"");
		
		Texture = Node.findChild(res);
		if (Texture){
			Shader.Uniformi(res,i);
			Texture.Bind(i);
			}
		}

	Mesh.UvCoords.Bind();
	Mesh.Normal.Bind();
	Mesh.Vertex.Bind();

	Con.Draw();

	Mesh.UvCoords.Unbind();
	Mesh.Normal.Unbind();
	Mesh.Vertex.Unbind();

	Shader.Unbind();

	gl.Screenshot(fn + "");
	Material.UnbindFBO();

	}






