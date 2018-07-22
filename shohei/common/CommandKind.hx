package;

@:enum
abstract CommandKind(Int)
{
	var Halt = 0;
	var Flip = 1;
	var Wait = 2;
	var SMove = 3;
	var LMove = 4;
	var Fission = 5;
	
	var Fill = 6;
	var SVoid = 7;
	
	var FusionP = 8;
	var FusionS = 9;
	
	var GFill = 10;
	var GVoid = 11;
	
	public function new(byte:Int)
	{
		var value = if (byte == 0xFF)
		{
			CommandKind.Halt;
		}
		else if (byte == 0xFE)
		{
			CommandKind.Wait;
		}
		else if (byte == 0xFD) 
		{
			CommandKind.Flip;
		}
		else if (byte & 0xF == 0x4)
		{
			CommandKind.SMove;
		}
		else if (byte & 0xF == 0xC)
		{
			CommandKind.LMove;
		}
		else if (byte & 0x7 == 0x5)
		{
			CommandKind.Fission;
		}
		else if (byte & 0x7 == 0x3)
		{
			CommandKind.Fill;
		}
		else if (byte & 0x7 == 0x7)
		{
			CommandKind.FusionP;
		}
		else if (byte & 0x7 == 0x6)
		{
			CommandKind.FusionS;
		}
		else if (byte & 0x7 == 0x2)
		{
			CommandKind.SVoid;
		}
		else if (byte & 0x7 == 0x1)
		{
			CommandKind.GFill;
		}
		else if (byte & 0x7 == 0x0)
		{
			CommandKind.GVoid;
		}
		else
		{
			throw "unknown command: " + byte;
		}
		
		this = value.toByte();
	}
	
	public function size():Int
	{
		return switch (this)
		{
			case CommandKind.Halt: 1;
			case CommandKind.Wait: 1;
			case CommandKind.Flip: 1;	
			case CommandKind.SMove: 2;	
			case CommandKind.LMove: 2;
			case CommandKind.FusionP: 1;
			case CommandKind.FusionS: 1;
			case CommandKind.Fission: 2;
			case CommandKind.Fill: 1;
			case CommandKind.SVoid: 1;
			case CommandKind.GFill: 4;
			case CommandKind.GVoid: 4;
			case _: 1;
		}
	}
	
	private inline function toByte():Int
	{
		return this;
	}
}
