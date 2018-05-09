/*
2018_REVISED
<NAME>Render2Normalmap</NAME>
<FILTER>Index</FILTER>
<DESCRIPTION>Renders the objectspaces normals into a texture</DESCRIPTION>
<XPM>
static char *NormalMap[]={
"16 16 11 1",
". c #000000",
"f c #0000c0",
"g c #008000",
"b c #008080",
"a c #00c000",
"d c #00ff00",
"i c #800080",
"c c #c00000",
"e c #c000c0",
"h c #ff00ff",
"# c #ffb618",
"................",
"....##aaaabb....",
"...##aaaaaabb...",
"..##a#aaaababb..",
"..###aaaaaabbb..",
"..###cccccccbb..",
"..###dd#d#ddbb..",
"e#e##db#dbddbfbf",
"ee#e##b#dbggfbff",
"eeee##b#dbggffff",
"eeee###hhaagffff",
".eeic##hhaaccff.",
"...ic##ccaacii..",
"...iieeaaeeiii..",
"...iieeeeeeii...",
".....######....."};
</XPM>
*/

v = "varying vec3 N;\n void main(void){ \n gl_Position = gl_MultiTexCoord0 * 2.0 - 1.0; \n N = gl_Normal; \n }";

f = "varying vec3 N; \n void main(void){gl_FragColor = vec4(normalize(N) * 0.5 + 0.5,1.0);}";

d = new Dialog;
d.caption = "Render to Objectnormalmap";
d.okButtonText = "Create";
d.cancelButtonText = "Cancel";

spin = new SpinBox;
spin.label = "Resolution: 2 power";
spin.minimum = 4;
spin.maximum = 12;
spin.value = 10;

label = new Label;
label.text="Cancel file selection to put to clipboard."
d.add(label);
d.add(spin);

if (d.exec()){
	fn = FileDialog.getSaveFileName( "*.png" );	
	Con = World.getContext();
	Mesh = Con.getParent();
	Node = Mesh.getParent();

	var node = World.addNode(World.timedString());	
	node.addScript();
	//old mess I guess ?  currently (2018) any engine has global gl object
    //gl = World.Temp.Script.gl;

	Texture = node.addTexture();
	Texture = node.Texture ;
	res = Math.pow(2,spin.value);
	Texture.Image2d(res,res,gl.RGB);

	Shader = gl.Shader(v,f);
	
	Texture.BindFBO();
	
	Shader.Bind();

	Mesh.UvCoords.Bind();
	Mesh.Normal.Bind();
	Mesh.Vertex.Bind();

	Con.Draw();

	Mesh.UvCoords.Unbind();
	Mesh.Normal.Unbind();
	Mesh.Vertex.Unbind();

	Shader.Unbind();
    if (typeof (fn) === "undefined"){ //user canceled dialog
        fn = ""
    }
	gl.Screenshot(fn + "");
	Texture.UnbindFBO();

	node.deleteLater();
}






