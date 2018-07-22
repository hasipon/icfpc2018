package;

abstract Far(Int) 
{
	public function new(value:Int)
	{
		this = value;
	}
	
	public var x(get, never):Int;
	private inline function get_x():Int 
	{
		return this & 0xFF - 30;
	}
	public var y(get, never):Int;
	private inline function get_y():Int 
	{
		return (this >> 8) & 0xFF - 30;
	}
	public var z(get, never):Int;
	private inline function get_z():Int 
	{
		return (this >> 16) & 0xFF - 30;
	}
	
	public function isPositive():Bool
	{
		return x >= 0 && y >= 0 && z >= 0;
	}
	
	public function toByte():Int
	{
		return this;
	}
}
