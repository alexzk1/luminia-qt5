/*
<NAME>Gear creator</NAME>
<FILTER>Node</FILTER>
<DESCRIPTION>Creates a gear mesh</DESCRIPTION>

<FUNCTION>createGearDialog()</FUNCTION>
<FUNCTION>createGear(int)</FUNCTION>

<ACTION>
<SLOT>createGearDialog()</SLOT>
<TEXT>Gear creator</TEXT>
<XPM>
static char *gear[]={
"16 16 5 1",
". c None",
"# c #000000",
"a c #c00000",
"b c #ff0000",
"c c #ffc0c0",
".......##.......",
"......#ab#......",
"...##.#ac#.##...",
"..#aa#aabb#ab#..",
"..#abaabbbbbc#..",
"...#abbccbbb#...",
".##abbc##bbbb##.",
"#aabbc#..#abbbc#",
"#abbbc#..#abbcc#",
".##bbbb##abbc##.",
"...#bbbaabbc#...",
"..#abbbbbbcbc#..",
"..#bc#bbbc#cc#..",
"...##.#bc#.##...",
"......#bc#......",
".......##......."};
</XPM>
</ACTION>
*/
function createGearDialog(){
	d = new Dialog;
	
	d.okButtonText = "Create";
	d.cancelButtonText = "Cancel";
	d.caption = "Create gear"
	
	t = new SpinBox;
	t.label = "Teeth";
	t.minimum = 6;
	t.maximum = 32;
	t.value = 8;
	d.add(t);

	if (d.exec()){
		createGear(t.value);
		}
	}
		

function createGear(teeth){

	if (teeth < 6) teeth = 6;
	if (teeth > 32) teeth = 32; 
	
	
	steps = teeth * 4;
	Ra = teeth + 1;
	Rb = teeth - 1.0;
	Rc = teeth - 2.0;
	Rd = teeth - 2.9;

	print ("Create Gear" );

	Gear = obj.addMesh("Gear"+teeth);
	Gear.addVertex();
// enum comptype{VERTEX, GENERIC, VECTOR, COLOR, UVCOORDS, BONEDEP, QUATERNION};
// bugfix hardcoded values
	Gear.addComponent(2,"Normal",3);
	Gear.addComponent(2,"Tangent",3);
	Gear.setNumOfVertices( 26 * steps);
	v = 0;
	with ( Math ){
		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)
			Gear.Vertex.set(v, si, ci, -1);
			Gear.Normal.set(v, -si, -ci,0);
			Gear.Tangent.set(v, ci, -si, 0);
			
			v ++;
			Gear.Vertex.set(v, si, ci, +1);
			Gear.Normal.set(v, -si, -ci,0);
			Gear.Tangent.set(v, ci, -si, 0);

			v ++;
			}

		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)
			Gear.Vertex.set(v, si, ci, +1);
			Gear.Normal.set(v,0,0,+1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			Gear.Vertex.set(v,2 * si, 2 * ci, +1);
			Gear.Normal.set(v, 0,0,1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;			
			}

		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)

			Gear.Vertex.set(v,2 * si, 2 * ci, +1);
			Gear.Normal.set(v, si * 0.707,ci * 0.707,0.707);
			Gear.Tangent.set(v, ci, -si, 0);

			v ++;
			Gear.Vertex.set(v,2.9 * si, 2.9 * ci, +0.1);
			Gear.Normal.set(v, si * 0.707,ci * 0.707,0.707);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			}
		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)
			Gear.Vertex.set(v,2.9 * si, 2.9 * ci, +0.1);
			Gear.Normal.set(v, 0,0,1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;

			Gear.Vertex.set(v, Rd * si, Rd * ci, +0.1);
			Gear.Normal.set(v, 0,0,1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			}

		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)
			Gear.Vertex.set(v, Rd * si, Rd * ci, +0.1);
			Gear.Normal.set(v, si * -0.707,ci * -0.707,0.707);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			Gear.Vertex.set(v, Rc * si, Rc * ci, +1.0);
			Gear.Normal.set(v, si * -0.707,ci * -0.707,0.707);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			}
		t = 0;
		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)
			Gear.Vertex.set(v, Rc * si, Rc * ci, +1.0);
			Gear.Normal.set(v, 0,0,1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;

			if (t < 2)Gear.Vertex.set(v,Rb * si, Rb * ci, +1.0);
			else      Gear.Vertex.set(v,Ra * si, Ra * ci, +1.0);

			Gear.Normal.set(v, 0,0,1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			t ++;
			if (t > 3)t =0;
			}



/****************************************************************************************/
		t = 0;		
		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi);
			sk = sin (phi + 2 * PI / steps); ck = cos(phi + 2 * PI / steps);

			switch (t){
				case 0: R0 = Rb; R1 = Rb; break;
				case 1: R0 = Rb; R1 = Ra; break;
				case 2: R0 = Ra; R1 = Ra; break;
				case 3: R0 = Ra; R1 = Rb; break;
				
				}
		

			Rsi = si * R0; Rci = ci * R0;
			Rsk = sk * R1; Rck = ck * R1;

			Gear.Vertex.set(v    , Rsi, Rci, +1.0);
			Gear.Vertex.set(v + 1, Rsi, Rci, -1.0);
			Gear.Vertex.set(v + 2, Rsk, Rck, +1.0);
			Gear.Vertex.set(v + 3, Rsk, Rck, -1.0);
			

			
			if (t == 0 || t == 2){
				Gear.Normal.set(v    , si,ci,0);
				Gear.Normal.set(v + 1, si,ci,0);
				Gear.Normal.set(v + 2, sk,ck,0);
				Gear.Normal.set(v + 3, sk,ck,0);
				}
			else {
				if (t == 1){
					a = 0.9;
					b = -0.43;
					}
				else {
					a = 0.9;
					b = 0.43;
					}
				Gear.Normal.set(v    , si * a + b * ci, ci * a - b * si,0);
				Gear.Normal.set(v + 1, si * a + b * ci, ci * a - b * si,0);
				Gear.Normal.set(v + 2, sk * a + b * ck, ck * a - b * sk,0);
				Gear.Normal.set(v + 3, sk * a + b * ck, ck * a - b * sk,0);

				}
			
			Gear.Tangent.set(v     , 0.0, 0.0, 1.0);
			Gear.Tangent.set(v + 1 , 0.0, 0.0, 1.0);
			Gear.Tangent.set(v + 2 , 0.0, 0.0, 1.0);
			Gear.Tangent.set(v + 3 , 0.0, 0.0, 1.0);
	
			
			v += 4;

			t ++;
			if (t > 3)t =0;
			}

/****************************************************************************************/
		t = 0;
		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)

			if (t < 2)Gear.Vertex.set(v,Rb * si, Rb * ci, -1.0);
			else      Gear.Vertex.set(v,Ra * si, Ra * ci, -1.0);
			Gear.Normal.set(v, 0,0,-1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;

			Gear.Vertex.set(v,Rc * si, Rc * ci, -1.0);
			Gear.Normal.set(v, 0,0,-1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;

			t ++;
			if (t > 3)t =0;
			}

		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)
			Gear.Vertex.set(v, Rc * si, Rc * ci, -1.0);
			Gear.Normal.set(v, si * -0.707,ci * -0.707,-0.707);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			Gear.Vertex.set(v, Rd * si, Rd * ci, -0.1);
			Gear.Normal.set(v, si * -0.707,ci * -0.707,-0.707);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;


			}
		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)

			Gear.Vertex.set(v,Rd * si, Rd * ci, -0.1);
			Gear.Normal.set(v, 0,0,-1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			Gear.Vertex.set(v,2.9 * si, 2.9 * ci, -0.1);
			Gear.Normal.set(v, 0,0,-1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			}


		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)

			Gear.Vertex.set(v,2.9 * si, 2.9 * ci, -0.1);
			Gear.Normal.set(v, si * 0.707,ci * 0.707,-0.707);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			Gear.Vertex.set(v,2 * si, 2 * ci, -1);
			Gear.Normal.set(v, si * 0.707,ci * 0.707,-0.707);
			Gear.Tangent.set(v, ci, -si, 0);

			v ++;
			}


		for (phi = PI / steps; phi < 2 * PI ; phi += 2 * PI / steps){
			si = sin (phi); ci=cos(phi)

			Gear.Vertex.set(v,2 * si, 2 * ci, -1);
			Gear.Normal.set(v, 0,0,-1);
			Gear.Tangent.set(v, ci, -si, 0);
			v ++;
			Gear.Vertex.set(v, si, ci, -1);
			Gear.Normal.set(v,0,0,-1);
			Gear.Tangent.set(v, ci, -si, 0);

			v ++;
			}
			
//****************************** index ***************************************
		Gear.addIndex("Index",4);
		step2 = steps * 2;
		for (i = 0; i < 6 ; i++ ){
			for ( k = 0 ; k <  steps; k++){
				v = (steps * i + k) * 2; 
				w = (steps * i + (k + 1)%steps) * 2; 
				Gear.Index.add( v , v + 1,  w +1 , w  );
				
				}
			} 
		for ( k = 0 ; k <  steps ; k++){
			v = k * 4 + 12 * steps;
			Gear.Index.add( v , v + 1, v + 3, v + 2);
			}
			
		for (i = 8; i < 13 ; i++ ){
			for ( k = 0 ; k <  steps; k++){
				v = (steps * i + k) * 2; 
				w = (steps * i + (k + 1)%steps) * 2; 
				Gear.Index.add( v , v + 1,  w +1 , w );
				
				}
			} 
		}
	}

