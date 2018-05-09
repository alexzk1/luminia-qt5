/*
2018_REVISED
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

World.destroyAll();
World.addNode("Wizard");
World.Wizard.addScript().load("media/relief.js");;
World.Wizard.addVertexshader().load("media/simpletex.vert");
World.Wizard.addFragmentshader().load("media/simpletex.frag");
World.Wizard.addTexture("Colormap").load("media/test5.png");
World.Wizard.addTexture("Normalmap");
World.Wizard.addTexture("Depthmap");

World.Wizard.createSphere(5);
World.Wizard.Script.run();