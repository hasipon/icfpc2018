package;

@:enum
abstract Direction(Int)
{
	var X = 1;
	var Y = 2;
	var Z = 3;
	
	public function new(value:Int)
	{
		this = value;
	}
}