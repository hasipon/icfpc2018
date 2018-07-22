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
			if (game.isStepTop)
			{
				currentStep = new StepData(
					game.energy,
					game.getPreviousActives()
				);
				stepLog.push(currentStep);
				game.startStep();
			}
			var command = Command.read(input);
			
			currentStep.commands.push(command);
			currentStep.backwardCommands.push(game.getBackwardCommand(command));
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
		return new Near(value);
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
		if (stepLog.length < nextIndex)
		{
			nextIndex = stepLog.length;
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
			var len = step.backwardCommands.length;
			
			trace(game.getActiveBotsCount(), len);
			if (game.getActiveBotsCount() != len)
			{
				throw stepLog[index] + "," + step.backwardCommands.length;
			}
			for (i in 0...len)
			{
				game.backward(step.backwardCommands[len- 1 - i]);
			}
			game.revertStep(
				step.energy,
				step.previousActives
			);
		}
	}
}

class StepData
{
	public var previousActives:Array<Bool>;
	public var energy:Int;
	public var commands:Array<Command>;
	public var backwardCommands:Array<BackwardCommand>;
	
	public function new (
		energy:Int,
		previousActives:Array<Bool>
	)
	{
		commands = [];
		backwardCommands = [];
		
		this.energy = energy;
		this.previousActives = previousActives;
	}
}
