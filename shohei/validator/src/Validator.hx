package;
import haxe.io.BytesInput;

class Validator 
{	
	public var energy:Float = 0;
	public var step:Int = 0;
	public var volatile:Volatile;

	public function new(game:Game, input:BytesInput) 
	{
		volatile = new Volatile(game);
		
		var i = 0;
		var time = Date.now().getTime();
		while (input.position < input.length)
		{
			if (game.isStepTop)
			{
				game.startStep();
				volatile.startStep(); // アクティブになったのを考慮するためgameより後
			}
			
			var command = Command.read(input);
			
			volatile.forward(command); // gameに適用前の座標で算出
			game.forward(command);
			
			if (i % 2000 == 0)
			{
				trace(input.position + "/"  + input.length + ":" + (Date.now().getTime() - time) + "ms");
			}
			i++;
		}
		
		energy = game.energy;
		step = game.step;
		
		if (!game.halted)
		{
			throw "Haltされていません";
		}
	}
}
