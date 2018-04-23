shader = gl.Shader(Vertexshader,Fragmentshader);
shader.Uniform1i("Tex0",0);

function render(){
	Texture.Bind(0);
	shader.Bind();

	Sphere.UvCoords.Bind();
	Sphere.Tangent.Bind(shader.Loc("Tangent"));
	Sphere.Vertex.Bind();
	Sphere.Normal.Bind();

	Sphere.Index.Draw(gl.QUADS);

	Sphere.UvCoords.Unbind();
	Sphere.Tangent.Unbind();
	Sphere.Vertex.Unbind();
	Sphere.Normal.Unbind();

	shader.Unbind();
	}