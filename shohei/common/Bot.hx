package;
import haxe.ds.Vector;

class Bot 
{
	public static var MAX:Int = 40;
	
	public var position:Position;
	
	public var id:BotId;
	public var seeds:Array<BotId>;
	
	public var isActive:Bool;
	public var isNextActive:Bool;
	
	public function new(id:Int, x:Int, y:Int, z:Int) 
	{
		this.id = new BotId(id);
		position = Position.fromXyz(x, y, z);
		isActive = false;
		isNextActive = false;
	}
	
	public function move(direction:Direction, length:Int):Void
	{
		this.position = this.position.move(direction, length);
	}
	
	public function forward():Void
	{
		isActive = isNextActive;
	}
}
