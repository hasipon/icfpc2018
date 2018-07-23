package;
import haxe.ds.Vector;

class Volatile 
{
	private var game:Game;
	public var data:Vector<Vector<Vector<Int>>>;
	
	public function new(game:Game) 
	{
		this.game = game;
		data = Game.createVector3D(game.size, -1);
	}
	
	public function startStep():Void
	{
		for (bot in game.bots)
		{
			if (bot.isActive)
			{
				lock(bot.position);
			}
		}
	}
	
	public function forward(command:Command):Void
	{
		var bot = game.getCurrentBot();
		switch (command.kind())
		{
			case CommandKind.Flip:
			case CommandKind.Wait:
				
			case CommandKind.LMove:
				lockLine(bot.position, command.direction1(), command.short1());
				lockLine(
					bot.position.move(command.direction1(), command.short1()),
					command.direction2(), 
					command.short2()
				);
				
			case CommandKind.SMove:
				lockLine(bot.position, command.direction1(), command.long());
				
			case CommandKind.Fission:
				lock(bot.position.near(command.nd()));
				
			case CommandKind.FusionP:
			case CommandKind.FusionS:
				
			case CommandKind.Fill | CommandKind.SVoid:
				lock(bot.position.near(command.nd()));
				
			case CommandKind.GFill | CommandKind.GVoid:
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
								lock(pos.moveXyz(x, y, z));
							}
						}
					}
				}

			case CommandKind.Halt:
		}
	}

	public inline function lock(position:Position):Void
	{
		if (position.x >= game.size) throw "x +方向にはみ出ました";
		if (position.y >= game.size) throw "y +方向にはみ出ました";
		if (position.z >= game.size) throw "z +方向にはみ出ました";
		if (position.x < 0) throw "x -方向にはみ出ました";
		if (position.y < 0) throw "y -方向にはみ出ました";
		if (position.z < 0) throw "z -方向にはみ出ました";
		
		if (isLocked(position))
		{
			throw "volatileが衝突しました。";
		}
		data[position.x][position.y][position.z] = game.step;
	}
	
	public inline function isLocked(position:Position):Bool
	{
		return data[position.x][position.y][position.z] == game.step;
	}
	
	public function isAccessable(position:Position):Bool
	{
		return 
			position.x < game.size &&
			position.y < game.size &&
			position.z < game.size &&
			game.currentModel[position.x][position.y][position.z] || isLocked(position);
	}
	
	public function lockLine(position:Position, dir:Direction, length:Int):Void
	{
		for (i in 0...length)
		{
			lock(position.move(dir, i + 1));
		}
	}
}
