/*
<FILTER>Texture</FILTER>
<FUNCTION>normalHistogrammDialog()</FUNCTION>

<ACTION>
<SLOT>normalHistogrammDialog()</SLOT>
<TEXT>Normal histogram BRDF factor texture</TEXT>
<XPM>
static char *gear[]={
"16 16 5 1",
". c None",
"# c #000000",
"a c #c00000",
"b c #ff0000",
"c c #ffc0c0",
"................",
".####........##.",
".####........##.",
".####........##.",
".####........##.",
".##.##.......##.",
".##..##......##.",
".##...##.....##.",
".##....##....##.",
".##.....##...##.",
".##......##..##.",
".##.......##.##.",
".##........####.",
".##........####.",
".##........####.",
"................"};
</XPM>
</ACTION>

*/

function changed(){
	if(cbox.checked){
		print("checked");
		concombo.enabled = true;


		}
	else{
		print("not checked");
		concombo.enabled = false;
		}
	sslider.label = "Blursize:" + sslider.value;

	}

function normalHistogrammDialog(){
	
	dialog = new Dialog();

	rescombo = new ComboBox();
	rescombo.itemList = Array("16","32","64","128","256","512","1024");
	rescombo.label = "Resolution:";
	dialog.add(rescombo);

	forcombo = new ComboBox();
	forcombo.itemList = Array("RGBA8","RGBA16F","RGBA32F");
	forcombo.label = "Format:";
	dialog.add(forcombo);

	sslider = new Slider();
	sslider.label = "Pointsize:";
	sslider.minimum = 1;
	sslider.maximum = 63; //pointsprite limit
	sslider.changed.connect(changed);

	dialog.add(sslider);
	

	scale = new NumberEdit();
	scale.decimals = 2;
	scale.value = 1.0;	
	scale.label = "Scale:";
	dialog.add(scale);

	cbox = new CheckBox()
	cbox.text = "Convolution texture";
	cbox.clicked.connect(changed);
	dialog.add(cbox);

	texturelist = World.findChildrenByType("Texture");
	texturenamelist = Array(texturelist.length);
	for(i = 0; i < texturelist.length; i++){
		texturenamelist[i] = texturelist[i].objectName;
		}

	concombo = new ComboBox();
	concombo.itemList = texturenamelist;
	concombo.enabled = false;
	concombo.label = "Texture:";
	dialog.add(concombo);


	if(dialog.exec()){
	
		ntex = obj.getParent().addTexture("NHistogram");
		ntex.Image2d(rescombo.currentItem, rescombo.currentItem,ntex.formatFromString(forcombo.currentItem));
			
		print(obj.formatFromString(forcombo.currentItem));


		width = obj.Width();

		tmp_mesh = obj.getParent().addMesh("tmpmesh", width);
		tmp_mesh.addComponent(0,"pos",1);

		height =  obj.Height();
		for (i = 0; i< height; i++){
			tmp_mesh.pos.set(i,i);
			}


vert = 	"#extension GL_EXT_gpu_shader4 : enable\n" +
	"uniform vec3 Tsize;\n" +
	"uniform sampler2D Src_tex;\n" +
	"void main(void){ \n"+
	"    vec3 normal = texture2D(Src_tex, vec2(gl_Vertex.x, gl_InstanceID)/Tsize.xy).xyz;\n" +
	"    normal = normalize (normal - 0.5);\n" +
	"    gl_Position.xy = normal.xy /(normal.z + 1.0);\n" +
	/*"    gl_Position.xy =  vec2(gl_Vertex.x, gl_InstanceID )/Tsize.xy;" +*/

	"    gl_Position.z = 0.0;\n" +
	"    gl_Position.w = 1.0;\n" +
	"    gl_PointSize = Tsize.z;\n" +
	"    }";


frag =  "uniform float scale;\n" +
	"void main(void){\n" +
	"    gl_FragColor = vec4(1.0,1.0,1.0,1.0) * scale;\n" +
	"    }" ;


frag2 = "uniform sampler2D kernel;\n" +
	"uniform float scale;\n" +
	"void main(void){\n" +
	"    gl_FragColor = texture2D(kernel, gl_PointCoord) * scale ;\n" +
	"    }";



		obj.Bind(0);
		if(cbox.checked){
			Shader = gl.Shader(vert, frag2);
			Shader.Bind();
			Shader.Uniformi("kernel",1);

			for(i = 0; i < texturelist.length;i++){
				if(texturelist[i].objectName == concombo.currentItem){
					texturelist[i].Bind(1);
					print(texturelist[i] + " " + texturelist[i].Width());
					}
	
				}


			}
		else{
			Shader = gl.Shader(vert, frag);
			Shader.Bind();
			}

		Shader.Uniformi("Src_tex",0);
		Shader.Uniform("Tsize",width,height,sslider.value );
		Shader.Uniform("scale", scale.value *  Math.pow(rescombo.currentItem,2) / (Math.pow(sslider.value,2) * width * height));




		tmp_mesh.pos.Bind(0);


		ntex.BindFBO();
		gl.Clear(gl.COLOR_BUFFER_BIT);



		gl.Enable(gl.BLEND);
	        gl.BlendFunc(gl.ONE,gl.ONE);

		if(cbox.checked){
			tmp_mesh.DrawInstanced(height, gl.POINT_SPRITE);
			}
		else{
			tmp_mesh.DrawInstanced(height, gl.POINTS);
			}


		gl.Disable(gl.BLEND);


		Shader.Unbind();
	

		gl.Screenshot("normal_histogramm.png");
		ntex.UnbindFBO();

	
		//enable blend
		tmp_mesh.pos.UnbindAll(); //fix the deconstrotur in Item_component !!!!
		tmp_mesh.deleteLater();
		}
	}
