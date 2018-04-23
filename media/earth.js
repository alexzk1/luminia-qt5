shader = gl.Shader(Vertexshader,Fragmentshader);
shader.Uniformi("Tex1",0);

function render(){
	gl.Rotate(60 * World.getTime(),0,1,0);
	Texture.Bind(0);
	shader.Bind();

	Sphere.UvCoords.Bind();
	Sphere.Tangent.Bind(shader.Loc("Tangent"));
	Sphere.Vertex.Bind();
	Sphere.Normal.Bind();

	Sphere.Index.Draw();

	Sphere.UvCoords.Unbind();
	Sphere.Tangent.Unbind();
	Sphere.Vertex.Unbind();
	Sphere.Normal.Unbind();

	shader.Unbind();
	}