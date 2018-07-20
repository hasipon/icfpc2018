package;
import haxe.ds.Vector;

class Bot 
{
	public var x:Int;
	public var y:Int;
	public var z:Int;
	
	public var id:Int;
	public var seeds:Vector<Bool>;

	public function new(id:Int, x:Int, y:Int, z:Int) 
	{
		this.id = id;
		this.y = y;
		this.z = z;
		this.x = x;
		seeds = new Vector(20);
	}
}