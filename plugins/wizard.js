/*
<NAME>Wizard</NAME>
<FILTER>World</FILTER>
<DESCRIPTION>Creates a basic envrionment</DESCRIPTION>
<XPM>
static char *wizzard[]={
"16 16 8 1",
". c None",
"e c #000000",
"d c #000080",
"c c #0000ff",
"f c #0080ff",
"# c #808000",
"a c #ffff00",
"b c #ffffff",
"....#...........",
"...#a#..........",
"...#a#..........",
".##aba##........",
"#aabbbaa#.......",
".##aba##........",
"...#a#cde.......",
"...#a#fcde......",
"....#.efcde.....",
".......efcde....",
"........efcde...",
".........efcde..",
"..........efcde.",
"...........efcde",
"............efce",
".............ee."};
</XPM>
*/


World.addNode("Wizard");
World.Wizard.addScript();
World.Wizard.addVertexshader();
World.Wizard.addFragmentshader();
World.Wizard.addTexture("Colormap");
World.Wizard.addTexture("Normalmap");
World.Wizard.addTexture("Depthmap");

World.Wizard.Colormap.load("media/test5.png");


World.Wizard.Fragmentshader.load("media/simpletex.frag");
World.Wizard.Script.load("media/relief.js");
World.Wizard.Vertexshader.load("media/simpletex.vert");
World.Wizard.createSphere(5);
World.Wizard.Script.run();