package;

abstract Far(Int) 
{
	public function new(value:Int)
	{
		this = value;
	}
	
	public static function fromXyz(x:Int, y:Int, z:Int):Far
	{
		return new Far(
			((x + 30) & 0xFF) +
			(((y + 30) & 0xFF) << 8) +
			(((z + 30) & 0xFF) << 16)
		);
	}
	
	public var x(get, never):Int;
	private inline function get_x():Int 
	{
		return (this & 0xFF) - 30;
	}
	public var y(get, never):Int;
	private inline function get_y():Int 
	{
		return ((this >> 8) & 0xFF) - 30;
	}
	public var z(get, never):Int;
	private inline function get_z():Int 
	{
		return ((this >> 16) & 0xFF) - 30;
	}
	
	public function isPositive():Bool
	{
		return x >= 0 && y >= 0 && z >= 0;
	}
	
	public function toByte():Int
	{
		return this;
	}
	
	public function toPositive():Far
	{
		return Far.fromXyz(
			if (x < 0) -x else x,
			if (y < 0) -y else y,
			if (z < 0) -z else z
		);
	}
}
