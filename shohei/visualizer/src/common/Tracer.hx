package;
import haxe.io.BytesInput;

class Tracer 
{
	public var game:Game;
	public var input:BytesInput;
	public var traceLog:Array<Command>;
	public var index:Int = 0;
	public var position:Float = 0;
	
	public function new(game:Game, input:BytesInput) 
	{
		this.game = game;
		
		game.init();
		traceLog = [];
		
		while (input.position < input.length)
		{
			var byte = input.readByte();
			var command = if (byte == 0xFF)
			{
				Command.Halt;
			}
			else if (byte == 0xFE)
			{
				Command.Wait;
			}
			else if (byte == 0xFD) 
			{
				Command.Flip;
			}
			else if (byte & 0xF == 0x4)
			{
				var byte2 = input.readByte();
				Command.SMove(
					getDirection((byte >> 4) & 0x3),
					byte2 - 15
				);
			}
			else if (byte & 0xF == 0xB)
			{
				var byte2 = input.readByte();
				Command.LMove(
					getDirection((byte >> 4) & 0x3),
					(byte2 & 0xF) - 5,
					getDirection((byte >> 6) & 0x3),
					((byte2 >> 4) & 0xF) - 5
				);
			}
			else if (byte & 0x7 == 0x5)
			{
				var byte2 = input.readByte();
				Command.Fission(
					getNear(byte >> 3),
					byte2
				);
			}
			else if (byte & 0x7 == 0x3)
			{
				Command.Fill(getNear(byte >> 3));
			}
			else if (byte & 0x7 == 0x7)
			{
				Command.FussionP(game.getBotId(getNear(byte >> 3)));
			}
			else if (byte & 0x7 == 0x6)
			{
				Command.FussionS;
			}
			else
			{
				throw "unknown command: " + byte;
			}
			
			traceLog.push(command);
			game.forward(command);
		}
		
		game.init();
	}
	
	
	
	private function getDirection(value:Int):Direction
	{
		return if (value == 1)
		{
			Direction.X;
		}
		else if (value == 2)
		{
			Direction.Y;
		}
		else if (value == 3)
		{
			Direction.Z;
		}
		else
		{
			throw "unknown direction:" + value;
		}
	}
	
	private function getNear(value:Int):Near
	{
		var x = Std.int(value / 9);
		value -= x * 9;
		var y = Std.int(value / 3);
		value -= y * 3;
		
		return new Near(
			x - 1,
			y - 1,
			value - 1
		);
	}
	
	public function goto(nextIndex:Int):Void
	{
		for (i in index...nextIndex)
		{
			game.forward(traceLog[i]);
		}
		
		position = nextIndex;
		index = nextIndex;
	}
	
	public function move(offset:Float):Void
	{
		position += offset;
		var nextIndex = Std.int(position);
		
		for (i in index...nextIndex)
		{
			game.forward(traceLog[i]);
		}
		index = nextIndex;
	}
}
