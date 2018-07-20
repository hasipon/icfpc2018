package;

enum Command
{
	Halt;
	Fill;
	Flip;
	SMove(direction:Direction, length:Int);
	LMove(direction0:Direction, length0:Int, direction1:Direction, length1:Int);
}