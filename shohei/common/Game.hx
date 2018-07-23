package;
import haxe.ds.Option;
import haxe.ds.Vector;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

class Game
{
	public var shouldResetUnionFind:Bool;
	public var highHarmonics:Bool;
	
	public var voidLogs:Array<Position>;
	public var fillLogs:Array<Position>;
	
	public var targetModelInput:Option<BytesInput>;
	public var sourceModelInput:Option<BytesInput>;
	
	public var bots:Array<Bot>;
	public var botIndex:Int;
	
	public var currentMinX:Int;
	public var currentMinY:Int;
	public var currentMinZ:Int;
	public var currentMaxX:Int;
	public var currentMaxY:Int;
	public var currentMaxZ:Int;
	public var targetMinX:Int;
	public var targetMinY:Int;
	public var targetMinZ:Int;
	public var targetMaxX:Int;
	public var targetMaxY:Int;
	public var targetMaxZ:Int;
	public var boundMinX:Int;
	public var boundMinY:Int;
	public var boundMinZ:Int;
	public var boundMaxX:Int;
	public var boundMaxY:Int;
	public var boundMaxZ:Int;
	
	public var unionFind:UnionFind;
	
	public var currentModel:Vector<Vector<Vector<Bool>>>;
	public var targetModel:Vector<Vector<Vector<Bool>>>;
	public var size:Int;
	public var energy:Float;
	public var step:Int;
	public var halted:Bool;
	public var smallUnionFind:UnionFind;
	
	public var reservedFusionP:Map<Position, BotId>;
	public var reservedFusionS:Map<Position, BotId>;
	public var gVoidLog:Map<Position, FarAndCount>;
	public var gFillLog:Map<Position, FarAndCount>;
	
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
		
		smallUnionFind = new UnionFind(27);
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
		
		currentMinX = size;
		currentMinY = size;
		currentMinZ = size;
		currentMaxX = 0;
		currentMaxY = 0;
		currentMaxZ = 0;
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
								if (currentMinX > x) currentMinX = x;
								if (currentMinY > y) currentMinY = y;
								if (currentMinZ > z) currentMinZ = z;
								if (currentMaxX < x) currentMaxX = x;
								if (currentMaxY < y) currentMaxY = y;
								if (currentMaxZ < z) currentMaxZ = z;
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
		
		boundMinX = if (targetMinX < currentMinX) targetMinX else currentMinX;
		boundMinY = 0; // 地面に接地させる
		boundMinZ = if (targetMinZ < currentMinZ) targetMinZ else currentMinZ;
		
		boundMaxX = if (targetMaxX > currentMaxX) targetMaxX else currentMaxX;
		boundMaxY = if (targetMaxY > currentMaxY) targetMaxY else currentMaxY;
		boundMaxZ = if (targetMaxZ > currentMaxZ) targetMaxZ else currentMaxZ;
		halted = false;
		
		gFillLog = new Map();
		gVoidLog = new Map();
		
		voidLogs = [];
		fillLogs = [];
		shouldResetUnionFind = true;
	}
	
	public function startStep():Void
	{	
		if (reservedFusionS.iterator().hasNext())
		{
			throw "未処理のFusionSがあります。";
		}
		if (reservedFusionP.iterator().hasNext())
		{
			throw "未処理のFusionPがあります。";
		}
		for (key in gFillLog.keys())
		{
			var log = gFillLog[key];
			var corner = log.far.getCorner();
			if (corner != log.count)
			{
				throw "GFillの角とボットの数が合いません。" + log.count + "/" + corner;
			}
			if (corner == 1)
			{
				throw "1点のGFillはできません";
			}
			gFillLog.remove(key);
		}
		for (key in gVoidLog.keys())
		{
			var log = gVoidLog[key];
			var corner = log.far.getCorner();
			if (corner != log.count)
			{
				throw "GVoidの角とボットの数が合いません。" + log.count + "/" + corner;
			}
			if (corner == 1)
			{
				throw "1点のGVoidはできません";
			}
			gVoidLog.remove(key);
		}
		
		// ---------------------------
		var sizeX = boundMaxX - boundMinX + 1;
		var sizeY = boundMaxY - boundMinY + 1 + 1;
		var sizeZ = boundMaxZ - boundMinZ + 1; // 地面分
		for (i in 0...voidLogs.length)
		{
			var pos = voidLogs.pop();
			if (!shouldResetUnionFind)
			{
				smallUnionFind.reset(27);
				
				for (dx in 0...2)
				{
					for (dy in 0...2)
					{
						for (dz in 0...2)
						{
							var pos2 = Position.fromXyz(
								pos.x + dx - 1,
								pos.y + dy - 1,
								pos.z + dz - 1
							);
							if (
								0 < pos.x + dx - 1 &&
								0 < pos.y + dy - 1 &&
								0 < pos.z + dz - 1 &&
								isInBound(pos2) &&
								currentModel[pos2.x][pos2.y][pos2.z]
							)
							{
								inline function localUnionValue(x:Int, y:Int, z:Int) {
									return x * 9 +  y * 3 + z;
								}
								if (0 <= dx - 1 && isInBound(Position.fromXyz(pos2.x - 1,pos2.y,pos2.z)) && currentModel[pos2.x - 1][pos2.y][pos2.z]) smallUnionFind.unionSet(localUnionValue(dx,dy,dz), localUnionValue(dx - 1,dy,dz));
								if (0 <= dy - 1 && isInBound(Position.fromXyz(pos2.x,pos2.y - 1,pos2.z)) && currentModel[pos2.x][pos2.y - 1][pos2.z]) smallUnionFind.unionSet(localUnionValue(dx,dy,dz), localUnionValue(dx,dy - 1,dz));
								if (0 <= dz - 1 && isInBound(Position.fromXyz(pos2.x,pos2.y,pos2.z - 1)) && currentModel[pos2.x][pos2.y][pos2.z - 1]) smallUnionFind.unionSet(localUnionValue(dx,dy,dz), localUnionValue(dx,dy,dz - 1));
							}
						}
					}
				}
				if (!smallUnionFind.isUnion())
				{
					shouldResetUnionFind = true;
				}
			}
		}
		for (i in 0...fillLogs.length)
		{
			var pos = fillLogs[i];
			var dx = pos.x - boundMinX;
			var dy = pos.y - boundMinY + 1;
			var dz = pos.z - boundMinZ; // 地面分
			
			#if !js
			connect(dx, dy, dz, sizeX, sizeY, sizeZ);
			#end
		}
		if (!highHarmonics)
		{
			#if !js
			if (shouldResetUnionFind)
			{
				resetUnionFind();
			}
			else 
			{
				for (i in 0...fillLogs.length)
				{
					var pos = fillLogs[i];
					var dx = pos.x - boundMinX;
					var dy = pos.y - boundMinY + 1; // 地面分
					var dz = pos.z - boundMinZ;
					if (!isGrounded(dx, dy, dz, sizeX, sizeY, sizeZ))
					{
						throw "新しいセルがグラウンドじゃありません。";
					}
				}
			}
			#end
		}
		for (i in 0...fillLogs.length)
		{
			fillLogs.pop();
		}
		
		// ---------------------------
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
		if (halted)
		{
			throw "すでにHaltしてます";
		}
		
		var bot = bots[botIndex];
		switch (command.kind())
		{
			case CommandKind.Flip:
				highHarmonics = !highHarmonics;
				shouldResetUnionFind = true;
				
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
				if (m < 0 || bot.seeds.length <= m)
				{
					throw "ボットID:" + bot.id  + "seedsが範囲外です。" + m + ":" + bot.seeds.length;
				}
				
				var id = bot.seeds.shift();
				var target = bots[id];
				
				target.position = bot.position.near(nd);
				target.isNextActive = true;
				target.seeds = bot.seeds.splice(0, m).copy(); 
				
				energy += 24;
				
			case CommandKind.FusionP:
				var selfPosition = bot.position;
				var nd = command.nd();
				if (reservedFusionS.exists(selfPosition))
				{
					var targetBot = bots[reservedFusionS[selfPosition]];
					if (bot.position.near(nd) != targetBot.position)
					{
						throw "FussionPが指す座標が、FissionSと一致してません";
					}
					fusion(
						bot, 
						targetBot
					);
					reservedFusionS.remove(selfPosition);
				}
				else
				{
					reservedFusionP[bot.position.near(nd)] = bot.id;
				}
				
			case CommandKind.FusionS:
				var selfPosition = bot.position;
				var nd = command.nd();
				if (reservedFusionP.exists(selfPosition))
				{
					var targetBot = bots[reservedFusionP[selfPosition]];
					if (bot.position.near(nd) != targetBot.position)
					{
						throw "FussionSが指す座標が、FissionPと一致してません";
					}
					fusion(
						targetBot,
						bot
					);
					reservedFusionP.remove(selfPosition);
				}
				else
				{
					reservedFusionS[bot.position.near(nd)] = bot.id;
				}
				
			case CommandKind.Fill:
				fill(bot.position.near(command.nd()));
				
			case CommandKind.SVoid:
				void(bot.position.near(command.nd()));
				
			case CommandKind.GFill:
				var far = command.far();
				var pos = bot.position.near(command.nd());
				var firstPos = pos.far(far.toFirst());
				var positive = far.toPositive();
				
				if (gFillLog.exists(firstPos))
				{
					var existingFar = gFillLog[firstPos];
					if (existingFar.far != positive)
					{
						throw "GFillの形が一致しません:" + bot.id;
					}
					existingFar.count += 1;
				}
				else
				{
					gFillLog[firstPos] = new FarAndCount(positive);
				}
				
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
				
				var pos = bot.position.near(command.nd());
				var firstPos = pos.far(far.toFirst());
				var positive = far.toPositive(); 
				
				if (gVoidLog.exists(firstPos))
				{
					var existingFar = gVoidLog[firstPos];
					if (existingFar.far != positive)
					{
						throw "GVoidの形が一致しません:" + bot.id;
					}
					existingFar.count += 1;
				}
				else
				{
					gVoidLog[firstPos] = new FarAndCount(positive);
				}
				
				if (far.isPositive())
				{
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
				halted = true;
				
				// 正解判定
				if (getActiveBotsCount() != 1)
				{
					throw "ボットが複数の状態でhaltしました:" + getActiveBotsCount();
				}
				if (bot.position.x != 0 || bot.position.y != 0 || bot.position.z != 0)
				{
					throw "原点以外でhaltしました:" + bot.position.x + "," + bot.position.y + "," + bot.position.z;
				}
				for (x in 0...size)
				{
					for (y in 0...size)
					{
						for (z in 0...size)
						{
							if (currentModel[x][y][z] != targetModel[x][y][z])
							{
								throw "モデルが完成してない状態でHaltしました:" + x + "," + y + "," + z;
							}
						}
					}
				}
		}
		
		botIndex += 1;
		while(botIndex < Bot.MAX)
		{
			if (bots[botIndex].isActive) break;
			botIndex += 1;
		}
	}
	
	
	public function isInBound(p:Position):Bool
	{
		return
			p.x < 0 ||
			p.y < 0 ||
			p.z < 0 ||
			p.x >= size ||  
			p.y >= size ||  
			p.z >= size;
	}
	public function checkBound(p:Position):Position
	{
		if (isInBound(p))
		{
			throw p.x + "," + p.y  + "," +  p.z + "は" + size + "の範囲外です";
		}
		
		return p;
	}
	
	public function fill(pos:Position):Void
	{
		checkBound(pos);
		if (!currentModel[pos.x][pos.y][pos.z])
		{
			energy += 12;
			currentModel[pos.x][pos.y][pos.z] = true;
		}
		else
		{
			energy += 6;
		}
		if (boundMinX > pos.x) { boundMinX = pos.x; shouldResetUnionFind = true; }
		if (boundMinY > pos.y) { boundMinY = pos.y; shouldResetUnionFind = true; }
		if (boundMinZ > pos.z) { boundMinZ = pos.z; shouldResetUnionFind = true; }
		if (boundMaxX < pos.x) { boundMaxX = pos.x; shouldResetUnionFind = true; }
		if (boundMaxY < pos.y) { boundMaxY = pos.y; shouldResetUnionFind = true; }
		if (boundMaxZ < pos.z) { boundMaxZ = pos.z; shouldResetUnionFind = true; }
		if (currentMinX > pos.x) { currentMinX = pos.x; }
		if (currentMinY > pos.y) { currentMinY = pos.y; }
		if (currentMinZ > pos.z) { currentMinZ = pos.z; }
		if (currentMaxX < pos.x) { currentMaxX = pos.x; }
		if (currentMaxY < pos.y) { currentMaxY = pos.y; }
		if (currentMaxZ < pos.z) { currentMaxZ = pos.z; }
		
		fillLogs.push(pos);
	}
	
	public function void(pos:Position):Void
	{
		checkBound(pos);
		if (currentModel[pos.x][pos.y][pos.z])
		{
			energy -= 12;
			currentModel[pos.x][pos.y][pos.z] = false;
		}
		else
		{
			energy += 3;
		}
		 voidLogs.push(pos);
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
				BackwardCommand.Halt;
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
	
	public function revertStep(energy:Float, previousActivates:Array<Bool>):Void
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
				
			case BackwardCommand.Halt:
				halted = false;
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
	
	@:generic
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
	
	
	public function resetUnionFind():Void
	{
		shouldResetUnionFind = false;
		var sizeX = boundMaxX - boundMinX + 1;
		var sizeY = boundMaxY - boundMinY + 1 + 1;
		var sizeZ = boundMaxZ - boundMinZ + 1; // 地面分
		
		unionFind = new UnionFind(
			sizeX * 
			sizeY * 
			sizeZ
		);
		
		for (dx in 0...sizeX)
		{
			for (dz in 0...sizeZ)
			{
				connect(dx, 0, dz, sizeX, sizeY, sizeZ); // 地面はつなげる
			}
			for (dy in 1...sizeY)
			{
				for (dz in 0...sizeZ)
				{
					var x = boundMinX + dx;
					var y = boundMinY + dy - 1;
					var z = boundMinZ + dz;
					
					if (currentModel[x][y][z])
					{
						connect(dx, dy, dz, sizeX, sizeY, sizeZ);
					}
				}
			}
		}
		
		if (!highHarmonics)
		{
			for (dx in 0...sizeX)
			{
				for (dy in 1...sizeY)
				{
					for (dz in 0...sizeZ)
					{
						var x = boundMinX + dx;
						var y = boundMinY + dy - 1;
						var z = boundMinZ + dz;
						
						if (currentModel[x][y][z])
						{
							var localGrounded = isGrounded(dx, dy, dz, sizeX, sizeY, sizeZ);
							if (!localGrounded)
							{
								throw "グラウンドじゃありません。";
							}
						}
					}
				}
			}
		}
	}
	
	public function connect(dx:Int, dy:Int, dz:Int, sizeX:Int, sizeY:Int, sizeZ:Int):Void
	{
		var x = boundMinX + dx;
		var y = boundMinY + dy - 1;
		var z = boundMinZ + dz;
		
		var center = getUnionValue(dx, dy, dz, sizeX, sizeY, sizeZ);
		if (dy == 0)
		{
			unionFind.unionSet(center, getUnionValue(0, 0, 0, sizeX, sizeY, sizeZ));
		}
		else 
		{
			if (dx > 0         && currentModel[x - 1][y][z]) unionFind.unionSet(center, getUnionValue(dx - 1, dy, dz, sizeX, sizeY, sizeZ));
			if (dy == 1        || currentModel[x][y - 1][z]) unionFind.unionSet(center, getUnionValue(dx, dy - 1, dz, sizeX, sizeY, sizeZ));
			if (dz > 0         && currentModel[x][y][z - 1]) unionFind.unionSet(center, getUnionValue(dx, dy, dz - 1, sizeX, sizeY, sizeZ));
			if (dx < sizeX - 1 && currentModel[x + 1][y][z]) unionFind.unionSet(center, getUnionValue(dx + 1, dy, dz, sizeX, sizeY, sizeZ));
			if (dy < sizeY - 1 && currentModel[x][y + 1][z]) unionFind.unionSet(center, getUnionValue(dx, dy + 1, dz, sizeX, sizeY, sizeZ));
			if (dz < sizeZ - 1 && currentModel[x][y][z + 1]) unionFind.unionSet(center, getUnionValue(dx, dy, dz + 1, sizeX, sizeY, sizeZ));
		}
	}
	
	public inline function getUnionValue(dx:Int, dy:Int, dz:Int, sizeX:Int, sizeY:Int, sizeZ:Int):Int
	{
		return 
			dx * sizeZ * sizeY +
			dy * sizeZ +
			dz;
	}
	
	public function isGrounded(dx:Int, dy:Int, dz:Int, sizeX:Int, sizeY:Int, sizeZ:Int):Bool
	{
		return unionFind.findSet(
			getUnionValue(0, 0, 0, sizeX, sizeY, sizeZ),
			getUnionValue(dx, dy, dz, sizeX, sizeY, sizeZ)
		);
	}
}

private class FarAndCount
{
	public var far:Far;
	public var count:Int;
	
	public function new(far:Far)
	{
		this.far = far;
		this.count = 1;
	}
}

private enum Grounded
{
	Yes;
	No;
	Unknown;
}
