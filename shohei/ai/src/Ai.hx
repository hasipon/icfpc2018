package;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

class Ai 
{
	public var game:Game;
	public var botAis:Array<BotAi>;
	public var bytesOutput:BytesOutput;
	
	public function new(input:BytesInput) 
	{
		game = new Game(input);
	}
	
	public function execute():Bytes
	{
		//target.seeds[i] = true; 
		//currentBot.seeds[i] = false;
		return null;
	}
}

class BotAi
{
	
}