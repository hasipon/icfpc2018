package;

abstract Near(Int)
{
	public var x(get, never):Int;
	private inline function get_x():Int 
	{
		return Std.int(this / 9) - 1;
	}
	
	public var y(get, never):Int;
	private inline function get_y():Int
	{
		return Std.int(this / 3) % 3 - 1;
	}
	
	public var z(get, never):Int;
	private inline function get_z():Int
	{
		return this % 3 - 1;
	}
	
	public function new(value:Int)
	{
		this = value;
	}
}
