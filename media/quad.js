shader = gl.Shader(Vertexshader,Fragmentshader);

function render(){
	shader.Bind();
	gl.Begin(gl.QUADS);
		gl.Vertex(1,0);
		gl.Vertex(0,1);
		gl.Vertex(-1,0);
		gl.Vertex(0,1);
	gl.End();
	shader.Unbind();
	}
