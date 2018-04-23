/*
<NAME>Test Particles</NAME>
<FILTER>Node</FILTER>
<DESCRIPTION>Creates a random particles or index meshes</DESCRIPTION>

<FUNCTION>createTestParticleDialog()</FUNCTION>

<ACTION>
<SLOT>createTestParticleDialog()</SLOT>
<TEXT>Create test particles</TEXT>
<XPM>
"16 16 5 1",
"# c None",
". c None",
"a c #000000",
"b c #0058c0",
"c c #ffffff",
"....###a######..",
"..####aba##a####",
".#a##abcbaaba##.",
".aba##abaabcba#.",
"abcba##a##aba###",
"#abaa###a##aa##.",
".#aaba#aba#aba#.",
".#abcbabcbabcba#",
".##aba#aba.aba##",
".#.aa##aa###a###",
".#aba#aba##aba##",
".abcbabcbaabcba.",
".#aba#abaa#aba##",
".##a###aaba#a###",
"..#####abcba####",
"..######aba#####"
</XPM> 
</ACTION>


*/

function createTestParticleDialog(){
d = new Dialog;

d.okButtonText = "Create";
d.cancelButtonText = "Cancel";
d.caption = "Simple Testobjects"

spin = new SpinBox;
spin.label = "Resolution: 2 power";
spin.minimum = 2;
spin.maximum = 16;
spin.value = 5;
d.add(spin);

var g = new GroupBox;
g.title = "Create:";
d.add( g );

var sphere = new RadioButton;
sphere.text = "Sphere";
sphere.tooltip = "Creates random position in a sphere volume"; 
sphere.checked = true;
g.add(sphere);

var cylinder = new RadioButton;
cylinder.tooltip = "Creates random position in a cylinder volume"; 
cylinder.text = "Cylinder";
g.add( cylinder);

var cube = new RadioButton;
cube.tooltip = "Creates random position in a cube volume"; 
cube.text = "Cube";
g.add(cube);

var indsquare = new RadioButton;
indsquare.tooltip = "Creates positions for indexing a square particle texture";
indsquare.text = "Square index";
g.add(indsquare);

var tri = new CheckBox;
tri.tooltip = "Generates 3 vertices instead one. The W component will be set to 1.0 2.0 or 3.0";
tri.text = "Triangles";
d.add(tri);


if (d.exec()){
	if (!tri.checked){
		if (sphere.checked){
			valu = Math.pow(2,spin.value);
			print ("Create Sphere" );
			print (valu);
			Sphere = obj.addMesh("Sphere");
			Sphere.addVertex();
			Sphere.setNumOfVertices(valu);
			with ( Math )for (v = 0;v < valu ; v++){
				do{
					x = random() * 2 -1;
					y = random() * 2 -1;
					z = random() * 2 -1;
				}while((x*x + y*y + z*z) > 1.0);
				Sphere.Vertex.set(v, x,y,z);
				}
			}
		else if (cylinder.checked){
			valu = Math.pow(2,spin.value);
			print ("Create Cylinder" );
			print (valu);
			Cylinder = obj.addMesh("Cylinder");
			Cylinder.addVertex();
			Cylinder.setNumOfVertices(valu);
			with ( Math )for (v = 0;v < valu ; v++){
				do{
					x = random() * 2 -1;
					y = random() * 2 -1;
					z = random() * 2 -1;
				}while((x*x + y*y) > 1.0);
				Cylinder.Vertex.set(v, x,y,z);
				}
			}
		else if (cube.checked){
			valu = Math.pow(2,spin.value);
			print ("Create Cube" );
			print (valu);
			Cube = obj.addMesh("Cube");
			Cube.addVertex();
			Cube.setNumOfVertices(valu);
			with ( Math )for (v = 0;v < valu ; v++){
				x = random() * 2 -1;
				y = random() * 2 -1;
				z = random() * 2 -1;
				Cube.Vertex.set(v, x,y,z);
				}
			}
		else if (indsquare.checked){
			valu = Math.pow(2,spin.value);
			print ("Create square index" );
			print (valu);
			Square = obj.addMesh("Square");
			Square.addVertex();
			Square.setNumOfVertices(valu * valu);
			n = 1 / valu
			v = 0;
			with ( Math )for (x = 0;x < 1 ; x += n )for (y = 0;y < 1 ; y += n ){
				Square.Vertex.set(v, x,y,0);
				v++;
				}
			}
		}
	else{
		if (sphere.checked){
			valu = Math.pow(2,spin.value);
			print ("Create Sphere" );
			print (valu);
			Sphere = obj.addMesh("Sphere");
			Sphere.addComponent(Sphere.VERTEX,"Vertex",4);
			Sphere.setNumOfVertices(valu*3);

			with ( Math )for (v = 0;v < valu*3 ; v+=3){
				do{
					x = random() * 2 -1;
					y = random() * 2 -1;
					z = random() * 2 -1;
					}while((x*x + y*y + z*z) > 1.0);
				Sphere.Vertex.set(v    , x, y, z, 1);
				Sphere.Vertex.set(v + 1, x, y, z, 2);
				Sphere.Vertex.set(v + 2, x, y, z, 3);
				}
			}
		else if (cylinder.checked){
			valu = Math.pow(2,spin.value);
			print ("Create Cylinder" );
			print (valu);
			Cylinder = obj.addMesh("Cylinder");
			Cylinder.addComponent(Cylinder.VERTEX,"Vertex",4);
			Cylinder.setNumOfVertices(valu*3);
			with ( Math )for (v = 0;v < valu*3 ; v+=3){
				do{
					x = random() * 2 -1;
					y = random() * 2 -1;
					z = random() * 2 -1;
					}while((x*x + y*y) > 1.0);
				Cylinder.Vertex.set(v    , x, y, z, 1);
				Cylinder.Vertex.set(v + 1, x, y, z, 2);
				Cylinder.Vertex.set(v + 2, x, y, z, 3);
				}
			}
		else if (cube.checked){
			valu = Math.pow(2,spin.value);
			print ("Create Cube" );
			print (valu);
			Cube = obj.addMesh("Cube");
			Cube.addComponent(Cube.VERTEX,"Vertex",4);
			Cube.setNumOfVertices(valu);
			with ( Math )for (v = 0;v < valu *3 ; v+=3){
				x = random() * 2 -1;
				y = random() * 2 -1;
				z = random() * 2 -1;
				Cube.Vertex.set(v    , x, y, z, 1);
				Cube.Vertex.set(v + 1, x, y, z, 2);
				Cube.Vertex.set(v + 2, x, y, z, 3);
				}
			}
		else if (indsquare.checked){
			valu = Math.pow(2,spin.value);
			print ("Create square index" );
			print (valu);
			Square = obj.addMesh("Square");
			Square.addComponent(Square.VERTEX,"Vertex",4);
			Square.setNumOfVertices(valu * valu * 3);
			n = 1 / valu
			v = 0;
			with ( Math )for (x = 0;x < 1 ; x += n )for (y = 0;y < 1 ; y += n ){
				Square.Vertex.set(v    , x, y, 0, 1);
				Square.Vertex.set(v + 1, x, y, 0, 2);
				Square.Vertex.set(v + 2, x, y, 0, 3);
				v+=3;
				}
			}
		}
	}
}