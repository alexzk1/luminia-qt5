/*
<NAME>Convolution Matrix</NAME>
<FILTER>Texture</FILTER>
<DESCRIPTION>Filter a texture with a 3x3 Convolutionmatrix</DESCRIPTION>

<FUNCTION>createConvolutionDialog()</FUNCTION>


<ACTION>
<SLOT>createConvolutionDialog()</SLOT>
<TEXT>Convolution Matrix</TEXT>
<XPM>
static char *convolution[]={
"16 16 3 1",
"# c #000000",
"a c #a0a0a0",
". c #c3c3c3",
"..#..aaa#aa..#..",
".#.#.aa##aa.#.#.",
".#.#.aaa#aa.#.#.",
"..#..aaa#aa..#..",
".....aaaaaa.....",
"aaaaa......aaaaa",
"aaa#a..#.#.aaa#a",
"aa##a..###.aa##a",
"aaa#a##..#.aaa#a",
"aaa#a....#.aaa#a",
"aaaaa......aaaaa",
".....aaaaaa.....",
"..#..aaa#aa..#..",
".#.#.aa##aa.#.#.",
".#.#.aaa#aa.#.#.",
"..#..aaa#aa..#.."};
</XPM>
</ACTION>
*/

function changed() {

	switch (combo.currentItem){
		case "Custom Normalized":
			sum = 0;
			for (i = 0; i < 9 ; i++)sum += v[i].value;
			for (i = 0; i < 9 ; i++) v[i].value = v[i].value/sum;
			break;
		case "Sharpen":
			v[0].value= 0; v[1].value=-1; v[2].value= 0;
			v[3].value=-1; v[4].value= 5; v[5].value=-1;
			v[6].value= 0; v[7].value=-1; v[8].value= 0;
			break;
		case "Blur":
			v[0].value= 0.1; v[1].value= 0.1; v[2].value= 0.1;
			v[3].value= 0.1; v[4].value= 0.2; v[5].value= 0.1;
			v[6].value= 0.1; v[7].value= 0.1; v[8].value= 0.1;
			break;
		case "Edge enhance":
			v[0].value= 0; v[1].value= 0; v[2].value= 0;
			v[3].value=-1; v[4].value= 1; v[5].value= 0;
			v[6].value= 0; v[7].value= 0; v[8].value= 0;
			break;
		case "Edge detect":
			v[0].value= 0; v[1].value= 1; v[2].value= 0;
			v[3].value= 1; v[4].value=-4; v[5].value= 1;
			v[6].value= 0; v[7].value= 1; v[8].value= 0;
			break;
		case "Emboss":
			v[0].value=-2; v[1].value=-1; v[2].value= 0;
			v[3].value=-1; v[4].value= 1; v[5].value= 1;
			v[6].value= 0; v[7].value= 1; v[8].value= 2;
			break;
		}
	}


function createConvolutionDialog(){

	d = new Dialog;
	d.caption = "Convolution Matrix";
	d.okButtonText = "Create";
	d.cancelButtonText = "Cancel";
	
	
	combo = new ComboBox;
	combo.itemList = Array("Custom","Custom Normalized","Sharpen","Blur","Edge enhance","Edge detect","Emboss");
	combo.label = "Presets:"
	d.add(combo);
	
	g = new GroupBox
	g.label = "Convolution Matrix";
	d.add(g);
	
	v = new Array;
	
	for (k = 0; k < 9 ;k += 3){
	for (i = k; i < k+3 ; i++){
		v[i]= new NumberEdit;
		g.add(v[i]);
		v[i].label = "";
		v[i].value = 0.0;
		}
	g.newColumn();
	}
	v[4].value = 1.0;



	combo.changed.connect(changed);

	if (d.exec()){
		createConvolution(v);


		}
	}



function createConvolution(v){

	Node = obj.getParent();
	
	Node.addTexture("Convolution");
	Out = Node.Convolution;
	
	Out.Image2d(obj.Width(),obj.Height(),gl.RGBA);

	vert = "void main(void){ \n gl_Position =  gl_Vertex * 2.0 - 1.0; \n gl_TexCoord[0]  = gl_Vertex; \n }";

	frag="uniform sampler2D Src;\n" +
	"const float  ox = 1.0/" + obj.Width() + ";\n" +
	"const float  oy = 1.0/" + obj.Height() + ";\n" +
	"void main(void){\n" +
   	"vec4 color = vec4(0.0,0.0,0.0,0.0);\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2(-ox,-oy)) * " + v[0].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2(0.0,-oy)) * " + v[1].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2( ox,-oy)) * " + v[2].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2(-ox,0.0)) * " + v[3].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy                ) * " + v[4].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2( ox,0.0)) * " + v[5].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2(-ox, oy)) * " + v[6].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2(0.0, oy)) * " + v[7].value + ";\n" +
	"color += texture2D(Src, gl_TexCoord[0].xy + vec2( ox, oy)) * " + v[8].value + ";\n" +
	"gl_FragColor = color;\n}\n";

	Shader = gl.Shader(vert,frag);
	
	Out.BindFBO();
	obj.Bind(0);
	Shader.Bind();

	gl.Begin(gl.QUADS);
		gl.Vertex(1,1);
		gl.Vertex(1,0);
		gl.Vertex(0,0);
		gl.Vertex(0,1);
	gl.End();

	Shader.Unbind();
	obj.Unbind();
	Out.UnbindFBO();
	}

