/*
<NAME>Light</NAME>
<FILTER>World</FILTER>
<DESCRIPTION>4 Lights</DESCRIPTION>

<FUNCTION>createLightDock()</FUNCTION>

<ACTION>
<SLOT>createLightDock()</SLOT>
<TEXT>4 Lights</TEXT>
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


MAXLIGHTS = 4;

c = Array(MAXLIGHTS);


salpha = Array(MAXLIGHTS);
sbeta = Array(MAXLIGHTS);

d = 0;

function createLightDock(){
	if(d != 0) return;

	d = new Dock();
	d.title = "Light Controll"
	for(i = 0; i < MAXLIGHTS; i++){
		c[i] = new ColorWidget();
		//sc[i].label = "Red";
		c[i].changed.connect(set);
		d.add(c[i]); 
	
	
		salpha[i] = new Slider();
		salpha[i].label = "Alpha";
		salpha[i].changed.connect(set);
		d.add(salpha[i]); 
	
		sbeta[i] = new Slider();
		sbeta[i].label = "Beta";
		sbeta[i].changed.connect(set);
		d.add(sbeta[i]); 
	
		if(i == 3 ){
			cb = new Button();
			cb.text = "Close" ;
			cb.clicked.connect(close);
			d.add(cb);
			}
		d.newColumn();
		}
	}

function close(){
	d.deleteLater();
	d = 0;
	}

function set(){

	for(i = 0; i < MAXLIGHTS; i++){


		gl.Light(i,gl.DIFFUSE,c[i].color);

		x = Math.sin(salpha[i].value/49 * Math.PI );
		z = Math.cos(salpha[i].value/49 * Math.PI );

		y = Math.cos(sbeta[i].value/99 * Math.PI );
		x *= Math.sin(sbeta[i].value/99 * Math.PI );
		z *= Math.sin(sbeta[i].value/99 * Math.PI );


		gl.Light(i,gl.POSITION,x,y,z,0);
		}

	World.setTime(World.getTime());


	}
