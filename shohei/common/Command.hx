package;
import haxe.ds.Vector;

enum Command
{
	Halt;
	Flip;
	Wait;
	SMove(direction:Direction, length:Int, fromX:Int, fromY:Int, fromZ:Int);
	LMove(direction0:Direction, length0:Int, direction1:Direction, length1:Int, fromX:Int, fromY:Int, fromZ:Int);
	Fission(nd:Near, m:Int);
	Fill(nd:Near);
	
	FussionP(id:Int, pSeeds:Vector<Bool>);
	FussionS;
}
