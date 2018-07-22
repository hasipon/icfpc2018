package;

class FissionAi implements AiState
{
	private var parent:Ai;
	private var game:Game;
	private var max:Int;
	
	public function new(parent:Ai) 
	{
		this.parent = parent;
		this.game = parent.game;
		var seg = Math.ceil(game.size / 30);
		seg = Math.floor((seg + 1) / 2);
		seg = seg * seg;
		seg *= 8;
		
		if (max > Bot.MAX)
		{
			max = Bot.MAX;
		}
	}
	
	public function step():StepResult
	{
		var activeCount = game.getActiveBotsCount();
		
		for (i in 0...200)
		{
			if (game.isStepTop)
			{
				game.startStep();
			}
			
			var bot = game.getCurrentBot();
			var command = Command.wait();
			
			if (bot.seeds.length > 0)
			{
				for (near in Near.all)
				{
					if (
						bot.position.isValidNear(near) &&
						parent.volatile.isAccessable(bot.position.near(near))
					)
					{
						command = Command.fission(
							near,
							Math.floor(bot.seeds.length / 2.5)
						);
						activeCount += 1;
						break;
					}
				}
			}
			parent.apply(command);
		}
		return StepResult.EndFission;
	}
}
