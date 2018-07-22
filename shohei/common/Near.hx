package;

abstract Near(Int)
{
	public static var all:Array<Near> = [
		Near.fromXyz(-1, -1,  0),
		Near.fromXyz( 0, -1, -1),
		Near.fromXyz(-1,  0, -1),
		Near.fromXyz(-1,  0,  0),
		Near.fromXyz( 0, -1,  0),
		Near.fromXyz( 0,  0, -1),
		
		
		Near.fromXyz( 1, -1,  0),
		Near.fromXyz( 0,  1, -1),
		Near.fromXyz( 1,  0, -1),
		
		Near.fromXyz(-1,  1,  0),
		Near.fromXyz( 0, -1,  1),
		Near.fromXyz(-1,  0,  1),
		
		Near.fromXyz( 1,  1,  0),
		Near.fromXyz( 0,  1,  1),
		Near.fromXyz( 1,  0,  1),
		Near.fromXyz( 1,  0,  0),
		Near.fromXyz( 0,  1,  0),
		Near.fromXyz( 0,  0,  1),
	];
	
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
	
	public static function fromXyz(x:Int, y:Int, z:Int):Near
	{
		return new Near(
			(x + 1) * 9 |
			(y + 1) * 3 |
			(z + 1)
		);
	}
	
	public function toByte():Int
	{
		return this;
	}
}
