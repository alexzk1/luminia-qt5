/*
<NAME>Normal2Quatmap</NAME>
<FILTER>Texture</FILTER>
<DESCRIPTION>Tool for converting normalmaps into quaternionmaps</DESCRIPTION>
<XPM>
"16 16 10 1",
"# c #000000",
"c c #0000c0",
"f c #008080",
"h c #00c000",
"g c #00ff00",
"d c #00ffff",
". c #585858",
"a c #58a8ff",
"e c #ff0000",
"b c #ffffff",
"..#.....##......",
".#a#...#bb##....",
"#cac#.#b##bb#...",
"#aaa#.#b#.##b#..",
".#a#.#d#b#ee#b#.",
".#a#.#d#b#ee#b#.",
".#a##d###b##.#b#",
".#a##d##e#bb##b#",
".#a#d##e####bb#.",
".#a#d#e###dd##..",
".#af#e##dd##....",
".#afe#dd##..##..",
".#aeff######gh#.",
".#ggggggggggggg#",
".###########gh#.",
"............##.."
</XPM>
*/

print ("q1");

vert = "void main(void){ \n"+
	"gl_Position =  gl_Vertex * 2.0 - 1.0; \n" +
	"gl_TexCoord[0]  = gl_Vertex; \n" +
	"}";

frag="uniform sampler2D Src;\n" +
	"void main(void){\n" +
	"vec3 N = normalize(texture2D(Src, gl_TexCoord[0].xy).xyz-0.5);\n" +
	"vec3 A = normalize(cross(N,vec3(0.0 ,0.0 ,1.0)));\n" +
	"float co = dot(normalize(N +vec3(0.0 ,0.0 ,1.0)),vec3(0.0 ,0.0 ,1.0));\n" +
	"float si = sqrt(1.0 - co*co);\n" +
	"gl_FragColor = vec4(A * si, co) * 0.5 + 0.5;\n" +
	//"gl_FragColor = vec4(A * si, co) * vec4(3.0,3.0,3.0,3.0) + vec4(-1.5,-1.5,-1.5, -2.0);\n" +
	"}\n";


frag="uniform sampler2D Src;\n"+
	"void main(void){\n"+
	"vec3 N = normalize(texture2D(Src, gl_TexCoord[0].xy).xyz-0.5 + vec3(0,0,3));\n"+
	"vec3 T = normalize(vec3(1.0,0.0,0.0) - N * dot(vec3(1.0,0.0,0.0),N)); \n"+
	"vec3 B = cross (N,T);\n"+

	"float t = 1.0 + T.x + B.y + N.z;\n"+

	"float S = sqrt(t)*2.0;\n"+
	"vec4 Q = vec4(  N.y - B.z , T.z - N.x, B.x - T.y, S * 0.25);\n"+
	"Q.xyz /= S;\n"+

	"gl_FragColor = normalize(Q)* 0.5 + 0.5;\n"+
	"}\n";


	Con = World.getContext();
	Node = Con.getParent();
	
	Out = Node.addTexture("Quaternionmap");
	Out.Image2d(Con.Width(),Con.Height(),gl.RGBA);

	Shader = gl.Shader(vert,frag);
	Shader.Uniformi("Src",14);
	Con.Bind(14);

	Shader.Bind();
	Out.BindFBO();

//	gl.Clear(gl.COLOR_BUFFER_BIT);
	gl.Begin(gl.QUADS);
		gl.Vertex(1,1);
		gl.Vertex(1,0);
		gl.Vertex(0,0);
		gl.Vertex(0,1);
	gl.End();

	gl.Screenshot("quatmap.png");
	Out.UnbindFBO();
	Shader.Unbind();
	Con.Unbind();





