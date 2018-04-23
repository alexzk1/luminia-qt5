shader = gl.Shader(Vertexshader,Fragmentshader);
shader.Uniform1i("Tex0",0);

function render(){
	Texture.Bind(0);
	shader.Bind();
	//shader.Uniform4f("col",1.0,0.5,0.0);
	gl.Begin(gl.TRIANGLES);
	  gl.TexCoord(0,0);
	  gl.Vertex(0, 0.5, 0);
	  gl.TexCoord(1,0);
	  gl.Vertex(0.5, -0.5, 0.0);
	  gl.TexCoord(1,1);
	  gl.Vertex(-0.5, -0.5, 0);

	gl.End();
	shader.Unbind();
	}
