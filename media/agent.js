
shader = gl.Shader(Vertexshader,Fragmentshader);
//shader.Uniform1i("Tex0",0);

function render(){
//	Texture.Bind(0);
	shader.Bind();

	obj_import.UvCoords.Bind();
	obj_import.Normal.Bind();
	obj_import.Vertex.Bind();

	obj_import.agent_a.Draw();

	obj_import.UvCoords.Unbind();
	obj_import.Normal.Unbind();
	obj_import.Vertex.Unbind();

	shader.Unbind();
	}