shader = gl.Shader(Vertexshader,Fragmentshader);
shader.Uniformi("Colormap",0);
shader.Uniformi("Normalmap",1);
shader.Uniformi("Depthmap",2);
tan = shader.Loc("Tangent");
Sphere = Gear12;

function render(){
	Colormap.Bind(0);
	Normalmap.Bind(1);
	Depthmap.Bind(2);
	gl.Rotate(60 * World.getTime(), 0,1,0);
	gl.Translate(-10,0,0);	
	gl.Rotate(20 * World.getTime(), 0,0,1);

	shader.Bind();

	shader.Uniform("col",1.0,0.9,0.4,0);

	Gear12.Tangent.Bind(tan);
	Gear12.Vertex.Bind();
	Gear12.Normal.Bind();

	Gear12.Index.Draw();

	Gear12.Tangent.Unbind();
	Gear12.Vertex.Unbind();
	Gear12.Normal.Unbind();

	gl.Rotate(-20 * World.getTime(), 0,0,1);
	gl.Translate(20,0,0);

	gl.Rotate(-30 * World.getTime(), 0,0,1);

	shader.Uniform("col",0.8,0.8,1.0,0);

	Gear8.Tangent.Bind(tan);
	Gear8.Vertex.Bind();
	Gear8.Normal.Bind();

	Gear8.Index.Draw();

	Gear8.Tangent.Unbind();
	Gear8.Vertex.Unbind();
	Gear8.Normal.Unbind();

	shader.Uniform("col",0.0,0.4,0.0,0);
	gl.Translate(0,0,-2);

	Gear16.Tangent.Bind(tan);
	Gear16.Vertex.Bind();
	Gear16.Normal.Bind();

	Gear16.Index.Draw();

	Gear16.Tangent.Unbind();
	Gear16.Vertex.Unbind();
	Gear16.Normal.Unbind();




	shader.Unbind();
	}
