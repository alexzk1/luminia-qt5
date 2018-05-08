/* 
<NAME>Lumina IRC Client</NAME>
<FILTER>ScriptToolBar</FILTER>
<DESCRIPTION>Simple IRC client. Connect to #lumina @ irc.freenode.net</DESCRIPTION>
<TOGGLE/>
<XPM>
static char *irc[]={
"22 22 3 1",
". c None",
"# c #000000",
"a c #00ff00",
"......................",
"......................",
"......................",
"......................",
".############...#####.",
".#aa##aaaaaa##.##aaa##",
".#aa##aaaaaaa###aaaaa#",
".#aa##aa####aa#aaa##a#",
".#aa##aa#..#aa#aa#####",
".#aa##aa####aa#aa#....",
".#aa##aaaaaaaa#aa#....",
".#aa##aaaaaaa##aa#....",
".#aa##aa##aa###aa#....",
".#aa##aa##aa###aa#....",
".#aa##aa###aa##aa##...",
".#aa##aa#.#aa##aaa####",
".#aa##aa#.##aa##aaaaa#",
".#aa##aa#..#aa###aaa##",
".########..####.#####.",
"......................",
"......................",
"......................"};
</XPM> */

var nick = Input.getText("Enter Nickname");

dock = new Dock;
dock.title = "Lumina IRC Client";

tcp = new TcpClient("irc.freenode.net:6667");

chanels = new Array;
num_of_chanels = 0;

function extract_nick(n){
	return n.substring(1).split("!")[0];
	}

function findTab(name){
	var c=0;
	for (i = 0; i < num_of_chanels; i++){
		if (chanels[i].cname == name){
			c = i;
			}
		}
	return c;
	}

function ready(){
	line = tcp.readLine().replace("\r","");
	line = line.replace("\n","");
	split = line.split(" ");

	if (split[0] == "PING"){
		tcp.sendLine(line.replace("PING","PONG"));
		return;
		}

	else if(split[1] == "PRIVMSG"){
		sn = extract_nick(split[0]);
		sl = line.replace(split[0] +" " + split[1]+ " " + split[2] + " :","");
		chanels[findTab(split[2])].out.append( "<"+sn+"> "+ sl ,0,0,128);
		}
	else if (split[1] == "JOIN"){
		n = split[2].replace(":","").toLowerCase();
		join(n);
		chanels[findTab(n)].out.append(extract_nick(split[0]) + " joined " + n ,0,90,0);
		}
	else if (split[1] == "PART"){
		n = split[2].replace(":","");
		chanels[findTab(n)].out.append(extract_nick(split[0]) + " leaved " + n ,0,90,0);
		}
	else {
		for (i = 0; i < num_of_chanels; i++){
			//print (split[4]);
			if (chanels[i].cname == split[4].toLowerCase()){
				r = split[0]+ " "+ split[1]+ " "+ split[2]+ " " + split[3] + " " + split[4]+" :"
				chanels[i].out.append("Users in " + chanels[i].cname + ": " +line.replace(r,""),128,00,00);
				return;
				}
			}
		chanels[0].out.append(line,60,60,60);
		}

	
	}

tcp.ready.connect(ready);

function returnHandler(){
	for (i = 0; i < num_of_chanels; i++){
		if (chanels[i].cname == dock.currentTab())
		c = chanels[i];
		}

	s = c.lineedit.text.split(" ");
	if(s[0]=="/join"){
		tcp.sendLine("JOIN "+s[1]+"\r\n");
		s = c.lineedit.text = "";
		return;
		}

	tcp.sendLine("PRIVMSG "+ dock.currentTab() +" :" + c.lineedit.text +"\r\n");
	c.out.append("<"+nick + "> " + c.lineedit.text);
	c.lineedit.text = "";
	}

function join(name){
	for (i = 0; i < num_of_chanels; i++){
		print (i);
		if (chanels[i].cname == name){
			print(chanels[i].cname +" " +name);
			return;
			}
		}
	num_of_chanels++;
	dock.newTab(name);
	c = new Object;
	c.cname = name;
	c.out = new TextEdit();
	c.out.readOnly = true;
	dock.add(c.out);
	c.lineedit = new LineEdit();
	dock.add(c.lineedit);
	c.lineedit.returnPressed.connect(returnHandler);
	chanels.push(c);
	}

tcp.sendLine("NICK "+ nick + "\r\n");
tcp.sendLine("USER bla blo bla blo\r\n");
join("#lumina"); //create the first tab
tcp.sendLine("JOIN #lumina\r\n");

