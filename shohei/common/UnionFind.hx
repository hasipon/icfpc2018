package;
import haxe.ds.Vector;

class UnionFind 
{
	public var data:Vector<Int>;
	public function new(size:Int) 
	{
		data = new Vector(size);
		reset();
	}
	
	public inline function unionSet(x:Int, y:Int):Void
	{
		x = root(x);
		y = root(y);
		
		if (x != y) {
			if (data[y] < data[x]) 
			{
				var tmp = x;
				x = y;
				y = tmp;
			}
			
			data[x] += data[y];
			data[y] = x;
		}
	}
	
	public inline function findSet(x:Int, y:Int):Bool
	{
		return root(x) == root(y);
	}
	public inline function root(x:Int):Int 
	{
		return data[x] < 0 ? x : data[x] = root(data[x]);
	}
	
	public inline function size(x:Int):Int 
	{
		return -data[root(x)];
	}
	
	public function reset():Void
	{
		for (i in 0...data.length)
		{
			data[i] = -1;
		}
	}
}
