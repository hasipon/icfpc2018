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
	
	public var reservedFusionP:Map<Position, Bot>;
	public var reservedFusionS:Map<Position, Bot>;

	public var isStepTop(get, never):Bool;
	private function get_isStepTop():Bool 
	{
		return Bot.MAX <= botIndex;
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
				trace(sourceModelInput);
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
				
				for (i in 0...Bot.MAX)
				{
					var target = bots[bot.seeds.shift()];
					target.position = bot.position.near(nd);
					target.isNextActive = true;
					target.seeds = bot.seeds.splice(0, m); 
				}
				energy += 24;
				
			case CommandKind.FusionP:
				var selfPosition = bot.position;
				if (reservedFusionS.exists(selfPosition))
				{
					fusion(
						bot, 
						reservedFusionS[selfPosition]
					);
				}
				else
				{
					var nd = command.nd();
					reservedFusionP[bot.position.near(nd)] = bot;
				}
				
			case CommandKind.FusionS:
				var selfPosition = bot.position;
				if (reservedFusionP.exists(selfPosition))
				{
					fusion(
						reservedFusionP[selfPosition],
						bot
					);
				}
				else
				{
					var nd = command.nd();
					reservedFusionS[bot.position.near(nd)] = bot;
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
					for (x in 0...far.x)
					{
						for (y in 0...far.y)
						{
							for (z in 0...far.z)
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
					for (x in 0...far.x)
					{
						for (y in 0...far.y)
						{
							for (z in 0...far.z)
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
				BackwardCommand.Empty;
				
			case CommandKind.Wait:
				BackwardCommand.Empty;
				
			case CommandKind.LMove:
				BackwardCommand.LMove(bot.position);
				
			case CommandKind.SMove:
				BackwardCommand.SMove(bot.position);
				
			case CommandKind.Fission:
				BackwardCommand.Fission(bots[bot.seeds[0]]);
				
			case CommandKind.FusionP:
				var selfPosition = bot.position;
				if (reservedFusionS.exists(selfPosition))
				{
					var s = reservedFusionS[selfPosition];
					BackwardCommand.Fusion(bot, s, bot.seeds.copy(), s.seeds.copy());
				}
				else
				{
					BackwardCommand.Empty;
				}
				
			case CommandKind.FusionS:
				var selfPosition = bot.position;
				if (reservedFusionP.exists(selfPosition))
				{
					var p = reservedFusionP[selfPosition];
					BackwardCommand.Fusion(p, bot, p.seeds.copy(), bot.seeds.copy());
				}
				else
				{
					BackwardCommand.Empty;
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
		for (i in 0...Bot.MAX)
		{
			var len = secondaryBot.seeds.length;
			for (i in 0...len)
			{
				primaryBot.seeds.push(secondaryBot.seeds.pop());
			}
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
			bot.isActive = previousActivates[i];
			bot.isNextActive = bot.isActive;
		}
		
		botIndex = Bot.MAX;
		this.energy = energy;
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
				fusion(bot, target);
				
			case BackwardCommand.Fusion(primary, secondary, primarySeeds, secondarySeeds):
				primary.seeds = primarySeeds;
				secondary.seeds = secondarySeeds;
				
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
	
	public function createHistory3D(pos:Position, far:Far):Vector<Vector<Vector<Bool>>>
	{
		var result = new Vector(size);
		for (x in 0...far.x)
		{
			result[x] = new Vector(size);
			for (y in 0...far.y)
			{
				result[x][y] = new Vector(size);
				for (z in 0...far.z)
				{
					result[x][y][z] = currentModel[pos.x + x][pos.y + y][pos.z + z];
				}
			}
		}
		return result;
	}
}
