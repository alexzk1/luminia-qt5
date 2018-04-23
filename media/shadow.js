shader = gl.Shader(Vertexshader,Fragmentshader);
tan = shader.Loc("Tangent");

Colormap.ImageDepth(1024,1024);


Colormap.BindFBO();

	gl.Clear(gl.DEPTH_BUFFER_BIT);

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




Colormap.UnbindFBO();
