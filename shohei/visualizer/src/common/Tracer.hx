package;
import haxe.io.BytesInput;

class Tracer 
{
	public var game:Game;
	public var input:BytesInput;
	
	public var stepLog:Array<StepData>;
	public var index:Int = 0;
	public var position:Float = 0;
	
	public function new(game:Game, input:BytesInput) 
	{
		this.game = game;
		
		game.init();
		stepLog = [];
		
		var currentStep = null;
		while (input.position < input.length)
		{
			trace(game.botIndex, game.isStepTop);
			if (game.isStepTop)
			{
				currentStep = new StepData(
					game.getPreviousActives()
				);
				stepLog.push(currentStep);
				game.startStep();
			}
			trace(currentStep);
			
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
				var bot = game.getCurrentBot();
				Command.SMove(
					getDirection((byte >> 4) & 0x3),
					byte2 - 15,
					bot.x,
					bot.y,
					bot.z
				);
			}
			else if (byte & 0xF == 0xC)
			{
				var byte2 = input.readByte();
				var bot = game.getCurrentBot();
				
				Command.LMove(
					getDirection((byte >> 4) & 0x3),
					(byte2 & 0xF) - 5,
					getDirection((byte >> 6) & 0x3),
					((byte2 >> 4) & 0xF) - 5,
					bot.x,
					bot.y,
					bot.z
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
				Command.FussionP(
					game.getNearBot(getNear(byte >> 3)).id,
					game.getCurrentBot().seeds.copy()
				);
			}
			else if (byte & 0x7 == 0x6)
			{
				Command.FussionS;
			}
			else
			{
				throw "unknown command: " + byte;
			}
			
			currentStep.commands.push(command);
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
		position = nextIndex;
		_goto(nextIndex);
	}
	
	public function move(offset:Float):Void
	{
		position += offset;
		_goto(Std.int(position));
	}
	
	private function _goto(nextIndex:Int):Void
	{
		if (stepLog.length <= nextIndex)
		{
			nextIndex = stepLog.length - 1;
			position = nextIndex;
		}
		else if (nextIndex < 0)
		{
			nextIndex = 0;
			position = 0;
		}
		
		while (index < nextIndex)
		{
			var step = stepLog[index];
			
			game.startStep();
			for (command in step.commands)
			{
				game.forward(command);
			}
			
			index++;
		}
		while (nextIndex < index)
		{
			index--;
			var step = stepLog[index];
			var len = step.commands.length;
			for (i in 0...len)
			{
				game.backward(step.commands[len- 1 - i]);
			}
			game.revertStep(
				step.previousActives
			);
		}
	}
}

class StepData
{
	public var previousActives:Array<Bool>;
	public var commands:Array<Command>;
	
	public function new (
		previousActives:Array<Bool>
	)
	{
		this.previousActives = previousActives;
		commands = [];
	}
}
