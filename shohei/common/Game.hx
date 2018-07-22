package;
import haxe.ds.Option;
import haxe.ds.Vector;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

class Game
{
	public var targetModelInput:Option<BytesInput>;
	public var sourceModelInput:Option<BytesInput>;
	
	public var bots:Array<Bot>;
	public var botIndex:Int;
	
	public var highHarmonics:Bool;
	
	public var sourceMinX:Int;
	public var sourceMinY:Int;
	public var sourceMinZ:Int;
	public var sourceMaxX:Int;
	public var sourceMaxY:Int;
	public var sourceMaxZ:Int;
	public var targetMinX:Int;
	public var targetMinY:Int;
	public var targetMinZ:Int;
	public var targetMaxX:Int;
	public var targetMaxY:Int;
	public var targetMaxZ:Int;

	public var currentModel:Vector<Vector<Vector<Bool>>>;
	public var targetModel:Vector<Vector<Vector<Bool>>>;
	public var size:Int;
	public var energy:Int;
	public var step:Int;
	
	public var reservedFusionP:Map<Position, BotId>;
	public var reservedFusionS:Map<Position, BotId>;

	public var isStepTop(get, never):Bool;
	private function get_isStepTop():Bool 
	{
		return Bot.MAX <= botIndex;
	}
	
	public function getActiveBotsCount():Int
	{
		var result = 0;
		for (bot in bots)
		{
			if (bot.isActive)
			{
				result += 1;
			}
		}
		return result;
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
			case Option.None:
		}
		switch (sourceModelInput)
		{
			case Option.Some(sourceModelInput):
				sourceModelInput.position = 0;
				size = sourceModelInput.readByte();
				
			case Option.None:
		}
		
		highHarmonics = false;
		bots = [for (i in 0...Bot.MAX) new Bot(i, 0, 0, 0)];
		bots[0].isActive = true;
		bots[0].isNextActive = true;
		bots[0].seeds = [for (i in 1...Bot.MAX) new BotId(i)];
		
		reservedFusionP = new Map();
		reservedFusionS = new Map();
		
		energy = 0;
		step = 0;
		botIndex = Bot.MAX;
		
		currentModel = createVector3D(size, false);
		targetModel  = createVector3D(size, false);
		
		sourceMinX = size;
		sourceMinY = size;
		sourceMinZ = size;
		sourceMaxX = 0;
		sourceMaxY = 0;
		sourceMaxZ = 0;
		targetMinX = size;
		targetMinY = size;
		targetMinZ = size;
		targetMaxX = 0;
		targetMaxY = 0;
		targetMaxZ = 0;
		
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
							var fill = restValue & (1 << (7 - restCount)) != 0;
							currentModel[x][y][z] = fill;
							
							if (fill)
							{
								if (sourceMinX > x) sourceMinX = x;
								if (sourceMinY > y) sourceMinY = y;
								if (sourceMinZ > z) sourceMinZ = z;
								if (sourceMaxX < x) sourceMaxX = x;
								if (sourceMaxY < y) sourceMaxY = y;
								if (sourceMaxZ < z) sourceMaxZ = z;
							}
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
							
							var fill = restValue & (1 << (7 - restCount)) != 0;
							targetModel[x][y][z] = fill;
							
							if (fill)
							{
								if (targetMinX > x) targetMinX = x;
								if (targetMinY > y) targetMinY = y;
								if (targetMinZ > z) targetMinZ = z;
								if (targetMaxY < x) targetMaxX = x;
								if (targetMaxX < y) targetMaxY = y;
								if (targetMaxZ < z) targetMaxZ = z;
							}
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
		while(botIndex < Bot.MAX)
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
		step++;
	}
	
	public function forward(command:Command):Void
	{
		var bot = bots[botIndex];
		switch (command.kind())
		{
			case CommandKind.Flip:
				highHarmonics = !highHarmonics;
				
			case CommandKind.Wait:
				
			case CommandKind.LMove:
				bot.move(command.direction1(), command.short1());
				bot.move(command.direction2(), command.short2());
				energy += 2 * (abs(command.short1()) + 2 + abs(command.short2()));
				
			case CommandKind.SMove:
				bot.move(command.direction1(), command.long());
				energy += 2 * abs(command.long());
				
			case CommandKind.Fission:
				var nd = command.nd();
				var m = command.m();
				var id = bot.seeds.shift();
				var target = bots[id];
				
				target.position = bot.position.near(nd);
				target.isNextActive = true;
				target.seeds = bot.seeds.splice(0, m).copy(); 
					
				energy += 24;
				
			case CommandKind.FusionP:
				var selfPosition = bot.position;
				if (reservedFusionS.exists(selfPosition))
				{
					fusion(
						bot, 
						bots[reservedFusionS[selfPosition]]
					);
					reservedFusionS.remove(selfPosition);
				}
				else
				{
					var nd = command.nd();
					reservedFusionP[bot.position.near(nd)] = bot.id;
				}
				
			case CommandKind.FusionS:
				var selfPosition = bot.position;
				if (reservedFusionP.exists(selfPosition))
				{
					fusion(
						bots[reservedFusionP[selfPosition]],
						bot
					);
					reservedFusionP.remove(selfPosition);
				}
				else
				{
					var nd = command.nd();
					reservedFusionS[bot.position.near(nd)] = bot.id;
				}
				
			case CommandKind.Fill:
				fill(bot.position.near(command.nd()));
				
			case CommandKind.SVoid:
				void(bot.position.near(command.nd()));
				
			case CommandKind.GFill:
				var far = command.far();
				if (far.isPositive())
				{
					var pos = bot.position.near(command.nd());
					
					for (x in 0...far.x + 1)
					{
						for (y in 0...far.y + 1)
						{
							for (z in 0...far.z + 1)
							{
								fill(pos.moveXyz(x, y, z));
							}
						}
					}
				}
				
			case CommandKind.GVoid:
				var far = command.far();
				if (far.isPositive())
				{
					var pos = bot.position.near(command.nd());
					for (x in 0...far.x + 1)
					{
						for (y in 0...far.y + 1)
						{
							for (z in 0...far.z + 1)
							{
								void(pos.moveXyz(x, y, z));
							}
						}
					}
				}

			case CommandKind.Halt:
		}
		
		botIndex += 1;
		while(botIndex < Bot.MAX)
		{
			if (bots[botIndex].isActive) break;
			botIndex += 1;
		}
	}
	
	public function fill(pos:Position):Void
	{
		currentModel[pos.x][pos.y][pos.z] = true;
		energy += 12;
	}
	
	public function void(pos:Position):Void
	{
		currentModel[pos.x][pos.y][pos.z] = false;
		energy -= 12;
	}
	
	public function getBackwardCommand(command:Command):BackwardCommand
	{
		var bot = getCurrentBot();
		return switch (command.kind())
		{
			case CommandKind.Flip:
				BackwardCommand.Flip;
				
			case CommandKind.Wait:
				BackwardCommand.Empty;
				
			case CommandKind.LMove:
				BackwardCommand.LMove(bot.position);
				
			case CommandKind.SMove:
				BackwardCommand.SMove(bot.position);
				
			case CommandKind.Fission:
				BackwardCommand.Fission(bot.seeds[0]);
				
			case CommandKind.FusionP:
				var selfPosition = bot.position;
				if (reservedFusionS.exists(selfPosition))
				{
					var s = reservedFusionS[selfPosition];
					BackwardCommand.Fusion(bot.id, s, bot.seeds.copy(), bots[s].seeds.copy());
				}
				else
				{
					BackwardCommand.ReservFusionP;
				}
				
			case CommandKind.FusionS:
				var selfPosition = bot.position;
				if (reservedFusionP.exists(selfPosition))
				{
					var p = reservedFusionP[selfPosition];
					BackwardCommand.Fusion(p, bot.id, bots[p].seeds.copy(), bot.seeds.copy());
				}
				else
				{
					BackwardCommand.ReservFusionS;
				}
				
			case CommandKind.Fill:
				BackwardCommand.Fill(command.nd());
				
			case CommandKind.SVoid:
				BackwardCommand.SVoid(command.nd());
				
			case CommandKind.GFill:
				var far = command.far();
				if (far.isPositive())
				{
					var pos = bot.position.near(command.nd());
					BackwardCommand.GFill(
						pos,
						createHistory3D(pos, far)
					);
				}
				else
				{
					BackwardCommand.Empty;
				}
				
			case CommandKind.GVoid:
				var far = command.far();
				if (far.isPositive())
				{
					var pos = bot.position.near(command.nd());
					BackwardCommand.GVoid(
						pos,
						createHistory3D(pos, far)
					);
				}
				else
				{
					BackwardCommand.Empty;
				}
				
			case CommandKind.Halt:
				BackwardCommand.Empty;
		}
	}
	
	private function fusion(primaryBot:Bot, secondaryBot:Bot):Void
	{
		var len = secondaryBot.seeds.length;
		for (i in 0...len)
		{
			primaryBot.seeds.push(secondaryBot.seeds.pop());
		}
		
		primaryBot.seeds.push(secondaryBot.id);
		primaryBot.seeds.sort(compare);
		
		energy -= 24;
		secondaryBot.isNextActive = false;
	}
	
	private static function compare(a:Int, b:Int):Int 
	{
		if (a < b) return -1;
		else if (a > b) return 1;
		return 0;
	}
	
	static inline function abs(value:Int):Int 
	{
		return if (value < 0) -value else value;
	}
	
	public function revertStep(energy:Int, previousActivates:Array<Bool>):Void
	{
		for (i in 0...bots.length)
		{
			var bot = bots[i];
			bot.isNextActive = bot.isActive;
			bot.isActive = previousActivates[i];
		}
		
		botIndex = Bot.MAX;
		this.energy = energy;
		step--;
	}
	
	public function backward(command:BackwardCommand):Void
	{
		botIndex -= 1;
		while(botIndex >= 0)
		{
			if (bots[botIndex].isActive) break;
			botIndex -= 1;
		}
		var bot = bots[botIndex];
		switch (command)
		{
			case BackwardCommand.Flip:
				highHarmonics = !highHarmonics;
				
			case BackwardCommand.Empty:
				
			case BackwardCommand.LMove(position):
				bot.position = position;
				
			case BackwardCommand.SMove(position):
				bot.position = position;
				
			case BackwardCommand.Fission(target):
				fusion(bot, bots[target]);
				
			case BackwardCommand.Fusion(primary, secondary, primarySeeds, secondarySeeds):
				bots[primary].seeds = primarySeeds.copy();
				bots[secondary].seeds = secondarySeeds.copy();
				
			case BackwardCommand.GFill(pos, history) | BackwardCommand.GVoid(pos, history):
				for (x in 0...history.length)
				{
					var plain = history[x];
					for (y in 0...plain.length)
					{
						var line = plain[y];
						for (z in 0...line.length)
						{
							currentModel[pos.x + x][pos.y + y][pos.z + z] = line[z];
						}
					}
				}
				
			case BackwardCommand.Fill(near):
				void(bot.position.near(near));
				
			case BackwardCommand.SVoid(near):
				fill(bot.position.near(near));
				
			case BackwardCommand.ReservFusionP:
				reservedFusionP.remove(bot.position);
				
			case BackwardCommand.ReservFusionS:
				reservedFusionS.remove(bot.position);
		}
	}
	
	public function getCurrentBot():Bot
	{
		return bots[botIndex];
	}
	
	public function getPreviousActives():Array<Bool>
	{
		return [for (bot in bots) bot.isActive];
	}
	
	public static function createVector3D<T>(size:Int, defaultValue:T):Vector<Vector<Vector<T>>>
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
	
	public function createHistory3D(pos:Position, far:Far):Vector<Vector<Vector<Bool>>>
	{
		var result = new Vector(far.x + 1);
		for (x in 0...far.x + 1)
		{
			result[x] = new Vector(far.y + 1);
			for (y in 0...far.y + 1)
			{
				result[x][y] = new Vector(far.z + 1);
				for (z in 0...far.z + 1)
				{
					result[x][y][z] = currentModel[pos.x + x][pos.y + y][pos.z + z];
				}
			}
		}
		return result;
	}
}
