var shader = gl.Shader(Vertexshader,Fragmentshader);
shader.Uniformi("Colormap",0);
//shader.Uniformi("Normalmap",1);
//shader.Uniformi("Depthmap",2);
var tan = shader.Loc("Tangent");

function render(){
	Colormap.Bind(0);
	//Normalmap.Bind(1);
	//Depthmap.Bind(2);
	var now = World.getTime();
	gl.Rotate(60 * now, 0, 1, 0);

	shader.Bind();

	WizardObj.UvCoords.Bind();
	WizardObj.Tangent.Bind(tan);
	WizardObj.Vertex.Bind();
	WizardObj.Normal.Bind();

	WizardObj.Index.Draw();

	WizardObj.UvCoords.Unbind();
	WizardObj.Tangent.Unbind();
	WizardObj.Vertex.Unbind();
	WizardObj.Normal.Unbind();

	shader.Unbind();
}