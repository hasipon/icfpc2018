package;
import haxe.ds.Option;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

class Ai 
{
	public var game:Game;
	public var volatile:Volatile;
	
	public var state:AiState;
	public var bytesOutput:BytesOutput;
	
	public function new(sourceModelInput:Option<BytesInput>, targetModelInput:Option<BytesInput>) 
	{
		game = new Game(
			sourceModelInput,
			targetModelInput
		);
		state = new FissionAi(this);
		volatile = new Volatile(game);
		bytesOutput = new BytesOutput();
	}
	
	public function execute():Bytes
	{
		while (state != null)
		{
			switch (state.step())
			{
				case StepResult.Continue:
					
				case StepResult.EndFission:
					break;
			}
		}
		return bytesOutput.getBytes();
	}
	
	public function apply(command:Command):Void
	{
		volatile.applyCommand(command);
		game.forward(command);
		command.write(bytesOutput);
		
		if (game.isStepTop)
		{
			volatile.applyBots();
		}
	}
}
