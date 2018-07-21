package;
import haxe.ds.Option;
import haxe.ds.Vector;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

class Game
{
	private var targetModelInput:Option<BytesInput>;
	private var sourceModelInput:Option<BytesInput>;
	
	public var bots:Array<Bot>;
	public var botIndex:Int;
	
	public var highHarmonics:Bool;
	public var volatiles:Vector<Vector<Vector<Int>>>;
	public var currentModel:Vector<Vector<Vector<Bool>>>;
	public var targetModel:Vector<Vector<Vector<Bool>>>;
	public var size:Int;
	public var energy:Int;
	public var step:Int;
	
	public var isStepTop(get, never):Bool;
	private function get_isStepTop():Bool 
	{
		return 20 <= botIndex;
	}
	
	public function new(sourceModelInput:Option<BytesInput>, targetModelInput:Option<BytesInput>) 
	{
		this.targetModelInput = targetModelInput;
		this.sourceModelInput = sourceModelInput;
		init();
	}
	public function init():Void
	{
		switch (targetModelInput)
		{
			case Option.Some(targetModelInput):
				targetModelInput.position = 0;
				size = targetModelInput.readByte();
					
				trace(targetModelInput);
			case Option.None:
		}
		switch (sourceModelInput)
		{
			case Option.Some(sourceModelInput):
				trace(sourceModelInput);
				sourceModelInput.position = 0;
				size = sourceModelInput.readByte();
				
			case Option.None:
		}
		trace(size);
				
		highHarmonics = false;
		bots = [for (i in 0...20) new Bot(i, 0, 0, 0)];
		bots[0].isActive = true;
		bots[0].isNextActive = true;
		for (i in 1...20)
		{
			bots[0].seeds[i] = true;
		}
		
		energy = 0;
		step = 0;
		botIndex = 20;
		
		volatiles    = createVector3D(size, 0);
		currentModel = createVector3D(size, false);
		targetModel  = createVector3D(size, false);
		
		switch (sourceModelInput)
		{
			case Option.Some(sourceModelInput):
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
								restValue = sourceModelInput.readByte();
								restCount = 8;
							}
							
							restCount--;
							currentModel[x][y][z] = restValue & (1 << (7 - restCount)) != 0;
						}
					}
				}
				
			case Option.None:
		}
		switch (targetModelInput)
		{
			case Option.Some(targetModelInput):
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
				
			case Option.None:
		}
	}
	
	public function startStep():Void
	{
		for (bot in bots)
		{
			bot.forward();
			if (bot.isActive)
			{
				energy += 20;
			}
		}
		botIndex = 0;
		while(botIndex < 20)
		{
			if (bots[botIndex].isActive) break;
			botIndex += 1;
		}
		
		if (highHarmonics)
		{
			energy += size * size * size * 30;
		}
		else
		{
			energy += size * size * size * 3;
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
				trace(botIndex, currentBot.x, currentBot.y, currentBot.z);
				currentBot.move(d0, l0);
				currentBot.move(d1, l1);
				trace(botIndex, currentBot.x, currentBot.y, currentBot.z);
				energy += 2 * (abs(l0) + 2 + abs(l1));
				
			case Command.SMove(d0, l0, _):
				trace(botIndex, currentBot.x, currentBot.y, currentBot.z);
				currentBot.move(d0, l0);
				trace(botIndex, currentBot.x, currentBot.y, currentBot.z);
				if (currentBot.z > size) throw "over";
				energy += 2 * abs(l0);
				
			case Command.Fission(nd, m):
				var count = 0;
				var target = null;
				for (i in 0...20)
				{
					if (currentBot.seeds[i])
					{
						if (count == 0)
						{
							target = bots[i];
							target.x = currentBot.x + nd.x;
							target.y = currentBot.y + nd.y;
							target.z = currentBot.z + nd.z;
							target.isNextActive = true;
						}
						else if (count < m + 1)
						{ 
							target.seeds[i] = true; 
							currentBot.seeds[i] = false;
						}
						count++;
					}
				}
				energy += 24;
				if (target == null)
				{
					throw "seedが空のボットが、Fissionしようとしました。";
				}
				
			case Command.FussionP(targetId, _):
				var target = bots[targetId];
				for (i in 0...20)
				{
					if (target.seeds[i])
					{
						target.seeds[i] = false;
						currentBot.seeds[i] = true;
					}
				}
				energy -= 24;
				target.isNextActive = false;
				
			case Command.FussionS:
				// 
				
			case Command.Fill(near):
				trace(size, currentBot.x, near.x, currentBot.y, near.y, currentBot.z, near.z);
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
	}
	
	static inline function abs(value:Int):Int 
	{
		return if (value < 0) -value else value;
	}
	
	public function revertStep(previousActivates:Array<Bool>):Void
	{
		for (i in 0...bots.length)
		{
			var bot = bots[i];
			if (bot.isActive)
			{
				energy -= 20;
			}
			
			bot.isActive = previousActivates[i];
			bot.isNextActive = bot.isActive;
		}
		
		botIndex = 20;
		
		if (highHarmonics)
		{
			energy -= size * size * size * 30;
		}
		else
		{
			energy -= size * size * size * 3;
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
		var currentBot = bots[botIndex];
		switch (command)
		{
			case Command.Flip:
				highHarmonics = !highHarmonics;
				
			case Command.Wait:
				
			case Command.LMove(d0, l0, d1, l1, x, y, z):
				currentBot.goto(x, y, z);
				energy -= 2 * (abs(l0) + 2 + abs(l1));
				
			case Command.SMove(d0, l0, x, y, z):
				currentBot.goto(x, y, z);
				energy -= 2 * abs(l0);
				
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
	
	public function getPreviousActives():Array<Bool>
	{
		return [for (bot in bots) bot.isActive];
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

