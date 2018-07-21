package;
import haxe.ds.Vector;

class Bot 
{
	public var x:Int;
	public var y:Int;
	public var z:Int;
	
	public var id:Int;
	public var seeds:Vector<Bool>;
	
	public var isActive:Bool;
	public var isNextActive:Bool;
	public var isPrevActive:Bool;
	
	public function new(id:Int, x:Int, y:Int, z:Int) 
	{
		this.id = id;
		this.y = y;
		this.z = z;
		this.x = x;
		seeds = new Vector(20);
		isActive = false;
		isNextActive = false;
	}
	
	public function move(direction:Direction, length:Int):Void
	{
		switch (direction)
		{
			case Direction.X:
				x += length;
				
			case Direction.Y:
				y += length;
				
			case Direction.Z:
				z += length;
		}
	}
	public function goto(x:Int, y:Int, z:Int):Void
	{
		this.y = y;
		this.z = z;
		this.x = x;
	}
	
	public function forward():Void
	{
		isPrevActive = isActive;
		isActive = isNextActive;
	}
	
	public function backward():Void
	{
		isNextActive = isActive;
		isActive = isPrevActive;
	}
}
