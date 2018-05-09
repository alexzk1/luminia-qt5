/*
2018_REVISED
<NAME>Test Objects</NAME>
<FILTER>Node</FILTER>
<DESCRIPTION>Creates a Sphere or Torus mesh</DESCRIPTION>

<FUNCTION>createTestObjectDialog()</FUNCTION>
<FUNCTION>QObject* createSphere(int)</FUNCTION>
<FUNCTION>QObject* createTorus(int)</FUNCTION>
<FUNCTION>QObject* createQuadric(int)</FUNCTION>

<ACTION>
<SLOT>createTestObjectDialog()</SLOT>
<TEXT>Test Objects</TEXT>
<XPM>
static char *test[]={
"16 16 15 1",
". c None",
"# c #000000",
"l c #000040",
"h c #000080",
"g c #0000c0",
"e c #0000ff",
"b c #0058c0",
"a c #0080ff",
"d c #58a8ff",
"j c #800000",
"i c #c00000",
"c c #c0ffff",
"f c #ff0000",
"k c #ffc0c0",
"m c #ffffff",
"....########....",
"..##aaaaaaab##..",
".#aacdddddbaab#.",
"#baaaaefggggabh#",
"#baaghiffhhaaab#",
"#baabjjfkflaaab#",
"#baabbbbbaacdbb#",
".#bbeggggdddbb#.",
"..##hhhgggbb##..",
"..#j########f#..",
".#jjiiiiffkmkf#.",
"#jjiiiiiffkmkff#",
"#jiiiiiifffkmkf#",
"#iiiiiiifffkmkf#",
".###iiiifffk###.",
"....########...."};
</XPM>
</ACTION>
*/

function createTestObjectDialog(){
	var d = new Dialog();	
	d.okButtonText = "Create";
	d.cancelButtonText = "Cancel";
	d.caption = "Simple Testobjects";	
	
	var spin = new SpinBox;
	spin.label = "Resolution: 2 power";
	spin.minimum = 2;
	spin.maximum = 8;
	spin.value = 5;
	
	d.add(spin);
	
	var g = new GroupBox;
	g.title = "Create:";
	d.add( g );
	
	var sphere = new RadioButton;
	sphere.text = "Sphere";
	sphere.checked = true;
	g.add(sphere);
	
	var torus = new RadioButton;
	torus.text = "Torus";
	g.add(torus);
	
	var quadric = new RadioButton;
	quadric.text = "Quadric";
	g.add(quadric);

	if (d.exec()){
		if (sphere.checked){
			createSphere(spin.value);
		}
		else if	(torus.checked){
			createTorus(spin.value);
		}
		else if	(quadric.checked){
			createQuadric(spin.value);
		}
	}
}


function createSphere(res){
	if(res <2) res = 2;
	if(res >8) res = 8;

	var valu = Math.pow(2, res);	
	var valu1 = valu +1 ;
	
	print ("Create Sphere" );
	print ('Valu: ',valu, 'Res: ', res);
	
	var Sphere = obj.addMesh("Sphere");
	Sphere.addVertex();
	Sphere.addUvCoords();
	// enum comptype{VERTEX, GENERIC, VECTOR, COLOR, UVCOORDS, BONEDEP, QUATERNION};
	// bugfix hardcoded values
	Sphere.addComponent(2,"Normal",3);
	Sphere.addComponent(2,"Tangent",3);
	Sphere.setNumOfVertices(valu1 * (valu + 2 )/2);

	var v = 0; 
	var i = 1;
	with ( Math )for (var y = 0;y <= valu ; y += 2) for (var x = 0;x <= valu ; x++){
		i=  2 * PI * (x / valu) ;
		var j=   PI *  (y / valu)  - PI/2 ;
		var si = sin (i); 
		var ci=cos(i); 
		var sj = sin(j); 
		var cj = cos(j);

		Sphere.Vertex.set(v, -cj * ci, sj, cj * si);
		Sphere.Normal.set(v, -cj * ci, sj, cj * si);
		Sphere.Tangent.set(v, -si, 0, -ci);
		Sphere.UvCoords.set(v, ( x / valu), ( y / valu));
		v ++;
	}

	Sphere.addIndex("Index",4);
	with ( Math )for(y = 0;y < valu /2 ; y ++) for(x = 0;x < valu ; x++){
		var tmp  = x +      valu1 * y;
		var tmp2 = x + 1 +  valu1 * y ;
		Sphere.Index.add (tmp ,tmp2 ,tmp2 + valu1, tmp + valu1 );
	}
	return Sphere;
}

function createTorus(res){
	if(res <2) res = 2;
	if(res >8) res = 8;

	var valu = Math.pow(2,res);
	
	var valu1 = valu +1 ;

	var Torus = obj.addMesh("Torus");
	Torus.addVertex();
	Torus.addUvCoords();
	// enum comptype{VERTEX, GENERIC, VECTOR, COLOR, UVCOORDS, BONEDEP, QUATERNION};
	// bugfix hardcoded values
	Torus.addComponent(2,"Normal",3);
	Torus.addComponent(2,"Tangent",3);

	Torus.setNumOfVertices(valu1 * valu1);

	var v = 0;
	var r = 1; var R = 2; var i = 1;
	with ( Math )for (var y = 0;y <= valu ; y ++) for (var x = 0;x <= valu ; x++){
		i=  2 * PI * (x / valu) ;
		var j=  2 * PI * (y / valu) ;
		var si = sin (i); var ci=cos(i); var sj = sin(j); var cj = cos(j);
		Torus.Vertex.set(v,(R + r * cj ) * ci,(R + r * cj ) * si ,r * sj); 
		Torus.Normal.set(v, cj * ci, cj * si , sj); 
		Torus.Tangent.set(v, -si,  ci,0); 
		Torus.UvCoords.set(v, ( x / valu ), ( y / valu ));
		v ++;
	}
	Torus.addIndex("Index",4);
	with ( Math )for(y = 0;y < valu ; y ++) for(x = 0;x < valu ; x++){

		var tmp  = x +      valu1 * y;
		var tmp2 = x + 1 +  valu1 * y ;
		Torus.Index.add (tmp ,tmp2 ,tmp2 + valu1, tmp + valu1 );
	}
	return Torus;
}

function createQuadric(res){
	if(res <2) res = 2;
	if(res >8) res = 8;

	var valu = Math.pow(2,res);
	var valu1 = valu +1 ;

	var Quadric = obj.addMesh("Quadric");
	Quadric.setNumOfVertices(valu1 * valu1);
	Quadric.addVertex(); // Only position required
	var v = 0;
	with ( Math )for (var y = 0;y <= valu ; y ++)for (var x = 0;x <= valu ; x++){
		var nx = x / valu;
		var ny = y / valu;
		Quadric.Vertex.set(v,nx, ny, 0);
		v++;
	}

	Quadric.addIndex("Index",4);
	with ( Math )for(y = 0;y < valu ; y ++) for(x = 0;x < valu ; x++){
		
		var tmp  = x +      valu1 * y;
		var tmp2 = x + 1 +  valu1 * y ;
		Quadric.Index.add (tmp ,tmp2 ,tmp2 + valu1, tmp + valu1 );
	}

	return Quadric;
}

