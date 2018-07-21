package;

enum Command
{
	Halt;
	Flip;
	Wait;
	SMove(direction:Direction, length:Int);
	LMove(direction0:Direction, length0:Int, direction1:Direction, length1:Int);
	Fission(nd:Near, m:Int);
	Fill(nd:Near);
	
	FussionP(id:Int);
	FussionS;
}