package;

abstract Position(Int)
{
	public function new(value:Int)
	{
		this = value;
	}
	
	public static function fromXyz(x:Int, y:Int, z:Int):Position
	{
		return new Position(x + (y << 8) + (z << 16));
	}
	
	public var x(get, never):Int;
	private inline function get_x():Int 
	{
		return this & 0xFF;
	}
	public var y(get, never):Int;
	private inline function get_y():Int 
	{
		return (this >> 8) & 0xFF;
	}
	public var z(get, never):Int;
	private inline function get_z():Int 
	{
		return (this >> 16) & 0xFF;
	}
	
	public function move(direction:Direction, length:Int):Position
	{
		return switch (direction)
		{
			case Direction.X:
				moveX(length);
				
			case Direction.Y:
				moveY(length);
				
			case Direction.Z:
				moveZ(length);
		}
	}
	
	public function moveXyz(x:Int, y:Int, z:Int):Position
	{
		return moveX(x).moveY(y).moveZ(z);
	}
	
	public function moveX(diff:Int):Position
	{
		var prev = this;
		var result = this + diff;
		if (result & 0xFFFFFF00 != this & 0xFFFFFF00) throw "out of cube: x: " + diff;
		return new Position(result);
	}
	public function moveY(diff:Int):Position
	{
		var prev = this;
		var result = this + (diff << 8);
		if (result & 0xFFFF00FF != this & 0xFFFF00FF) throw "out of cube: y: " + diff;
		return new Position(result);
	}
	public function moveZ(diff:Int):Position
	{
		var prev = this;
		var result = this + (diff << 16);
		if (result & 0xFF00FFFF != this & 0xFF00FFFF) throw "out of cube: z: " + diff;
		return new Position(result);
	}
	
	public function near(nd:Near):Position
	{
		return moveX(nd.x).moveY(nd.y).moveZ(nd.z);
	}
	public function far(f:Far):Position
	{
		return moveX(f.x).moveY(f.y).moveZ(f.z);
	}
	
	public function isValidNear(nd:Near):Bool
	{
		return !(
			((this +  nd.x       ) & 0xFFFFFF00 != this & 0xFFFFFF00) ||
			((this + (nd.y <<  8)) & 0xFFFF00FF != this & 0xFFFF00FF) ||
			((this + (nd.z << 16)) & 0xFF00FFFF != this & 0xFF00FFFF)
		);
	}
}
