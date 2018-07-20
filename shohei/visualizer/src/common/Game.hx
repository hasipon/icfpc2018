package;
import haxe.ds.Vector;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

class Game 
{
	public var bots:Array<Bot>;
	public var activated:Array<Bool>;
	public var nextActivated:Array<Bool>;
	
	public var highHarmonics:Bool;
	public var volatiles:Vector<Vector<Vector<Int>>>;
	public var currentModel:Vector<Vector<Vector<Bool>>>;
	public var targetModel:Vector<Vector<Vector<Bool>>>;
	public var size:Int;
	public var energy:Int;
	public var traceData:BytesOutput;
	public var step:Int;
	
	public function new(targetModelInput:BytesInput) 
	{
		highHarmonics = false;
		size = targetModelInput.readByte();
		bots = [for (i in 0...20) new Bot(i, 0, 0, 0)];
		activated = [for (i in 0...20) false];
		nextActivated = [for (i in 0...20) true];
		activated[0] = true;
		nextActivated[0] = true;
		for (i in 0...20)
		{
			bots[0].seeds[i] = true;
		}
		
		energy = 0;
		step = 0;
		volatiles    = createVector3D(size, 0);
		currentModel = createVector3D(size, false);
		targetModel  = createVector3D(size, false);
		
		var restCount = 0;
		var restValue = 0;
		
		for (x in 0...size)
		{
			for (y in 0...size)
			{
				for (z in 0...size)
				{
					if (restCount == 0)
					{
						restValue = targetModelInput.readByte();
						restCount = 8;
					}
					
					restCount--;
					targetModel[x][y][z] = restValue & (1 << (7 - restCount)) != 0;
				}
			}
		}
		traceData = new BytesOutput();
	}
	
	private static function createVector3D<T>(size:Int, defaultValue:T):Vector<Vector<Vector<T>>>
	{
		var result = new Vector(size);
		for (i in 0...size)
		{
			result[i] = new Vector(size);
			for (j in 0...size)
			{
				result[i][j] = new Vector(size);
				for (k in 0...size)
				{
					result[i][j][k] = defaultValue;
				}
			}
		}
		return result;
	}
}

