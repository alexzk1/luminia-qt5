var shader = gl.Shader(Vertexshader,Fragmentshader);
shader.Uniformi("Colormap",0);
shader.Uniformi("Normalmap",1);
shader.Uniformi("Depthmap",2);
var tan = shader.Loc("Tangent");

function render(){
	Colormap.Bind(0);
	//Normalmap.Bind(1);
	//Depthmap.Bind(2);
	gl.Rotate(60 * World.getTime(), 0,1,0);

	shader.Bind();

	Sphere.UvCoords.Bind();
	Sphere.Tangent.Bind(tan);
	Sphere.Vertex.Bind();
	Sphere.Normal.Bind();

	Sphere.Index.Draw();

	Sphere.UvCoords.Unbind();
	Sphere.Tangent.Unbind();
	Sphere.Vertex.Unbind();
	Sphere.Normal.Unbind();

	shader.Unbind();
}