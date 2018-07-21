package;
import haxe.ds.Vector;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

class Game 
{
	private var targetModelInput:BytesInput;
	
	public var bots:Array<Bot>;
	public var botIndex:Int;
	
	public var highHarmonics:Bool;
	public var volatiles:Vector<Vector<Vector<Int>>>;
	public var currentModel:Vector<Vector<Vector<Bool>>>;
	public var targetModel:Vector<Vector<Vector<Bool>>>;
	public var size:Int;
	public var energy:Int;
	public var step:Int;
	
	public function new(targetModelInput:BytesInput) 
	{
		this.targetModelInput = targetModelInput;
		init();
	}
	public function init():Void
	{
		highHarmonics = false;
		targetModelInput.position = 0;
		size = targetModelInput.readByte();
		bots = [for (i in 0...20) new Bot(i, 0, 0, 0)];
		bots[0].isActive = true;
		bots[0].isNextActive = true;
		for (i in 0...20)
		{
			bots[0].seeds[i] = true;
		}
		
		energy = 0;
		step = 0;
		botIndex = 0;
		
		volatiles    = createVector3D(size, 0);
		currentModel = createVector3D(size, false);
		targetModel  = createVector3D(size, false);
		
		var restCount = 0;
		var restValue = 0;
		
		for (x in 0...size)
		{
			for (y in 0...size)
			{
				for (z in 0...size)
				{
					if (restCount == 0)
					{
						restValue = targetModelInput.readByte();
						restCount = 8;
					}
					
					restCount--;
					targetModel[x][y][z] = restValue & (1 << (7 - restCount)) != 0;
				}
			}
		}
	}
	
	public function forward(command:Command):Void
	{
		var currentBot = bots[botIndex];
		switch (command)
		{
			case Command.Flip:
				highHarmonics = !highHarmonics;
				
			case Command.Wait:
				
			case Command.LMove(d0, l0, d1, l1, _):
				currentBot.move(d0, l0);
				currentBot.move(d1, l1);
				energy += 2 * (l0 + 2 + l1);
				
			case Command.SMove(d0, l0, _):
				currentBot.move(d0, l0);
				energy += 2 * l0;
				
			case Command.Fission(_):
			case Command.FussionP(_):
			case Command.FussionS:
			case Command.Fill(near):
				currentModel[currentBot.x + near.x][currentBot.y + near.y][currentBot.z + near.z] = true;
				energy += 12;
				
			case Command.Halt:
		}
		
		botIndex += 1;
		while(botIndex < 20)
		{
			if (bots[botIndex].isActive) break;
			botIndex += 1;
		}
		if (botIndex == 20)
		{
			botIndex = 0;
			for (bot in bots)
			{
				bot.forward();
			}
			step++;
			if (highHarmonics)
			{
				energy += 30 * size * size * size;
			}
			else
			{
				energy += 3 * size * size * size;
			}
		}
		energy += 20;
		while(botIndex < 20)
		{
			if (bots[botIndex].isActive) break;
			botIndex += 1;
		}
	}
	
	public function backward(command:Command):Void
	{
		botIndex -= 1;
		while(botIndex >= 0)
		{
			if (bots[botIndex].isActive) break;
			botIndex -= 1;
		}
		if (botIndex == -1)
		{
			botIndex = 19;
			var activeCount = 0;
			
			for (bot in bots)
			{
				if (bot.isActive)
				{
					
				}
				bot.backward();
			}
			step--;
			if (highHarmonics)
			{
				energy -= 30 * size * size * size;
			}
			else
			{
				energy -= 3 * size * size * size;
			}
			energy -= 20;
		}
		while(botIndex >= 0)
		{
			if (bots[botIndex].isActive) break;
			botIndex -= 1;
		}
		
		var currentBot = bots[botIndex];
		switch (command)
		{
			case Command.Flip:
				highHarmonics = !highHarmonics;
				
			case Command.Wait:
				
			case Command.LMove(d0, l0, d1, l1, x, y, z):
				currentBot.goto(x, y, z);
				energy -= 2 * (l0 + 2 + l1);
				
			case Command.SMove(d0, l0, x, y, z):
				currentBot.goto(x, y, z);
				energy -= 2 * l0;
				
			case Command.Fission(_):
			case Command.FussionP(_):
			case Command.FussionS:
			case Command.Fill(near):
				currentModel[currentBot.x + near.x][currentBot.y + near.y][currentBot.z + near.z] = false;
				energy -= 12;
				
			case Command.Halt:
		}
	}
	
	public function getNearBot(near:Near):Bot
	{
		var bot = bots[botIndex];
		var tx = bot.x + near.x;
		var ty = bot.y + near.y;
		var tz = bot.z + near.z;
		
		for (target in bots)
		{
			if (target.isActive && target.x == tx && target.y == ty && target.z == tz)
			{
				return bot;
			}
		}
		
		throw 'bot not found at $tx, $ty, $tz';
	}
	
	public function getCurrentBot():Bot
	{
		return bots[botIndex];
	}
	
	private static function createVector3D<T>(size:Int, defaultValue:T):Vector<Vector<Vector<T>>>
	{
		var result = new Vector(size);
		for (i in 0...size)
		{
			result[i] = new Vector(size);
			for (j in 0...size)
			{
				result[i][j] = new Vector(size);
				for (k in 0...size)
				{
					result[i][j][k] = defaultValue;
				}
			}
		}
		return result;
	}
}

