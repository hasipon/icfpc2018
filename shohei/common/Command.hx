package;
import haxe.ds.Vector;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;

abstract Command(Int)
{
	private function new(value:Int)
	{
		this = value;
	}
	
	public static function read(input:BytesInput):Command
	{
		var value = input.readByte();
		var kind = new CommandKind(value);
		
		for (i in 1...kind.size())
		{
			value |= input.readByte() << (8 * i);
		}
		return new Command(value);
	}
	
	public function write(bytesOutput:BytesOutput):Void
	{
		for (i in 0...kind().size())
		{
			bytesOutput.writeByte(this >> (8 * i));
		}
	}
	
	public function kind():CommandKind
	{
		return new CommandKind(this & 0xFF);
	}
	
	public function nd():Near
	{
		return new Near((this >> 3) & 0x1F);
	}
	public function m():Int
	{
		return (this >> 8) & 0xFF;
	}
	public function direction1():Direction
	{
		return new Direction((this >> 4) & 0x03);
	}
	public function direction2():Direction
	{
		return new Direction((this >> 6) & 0x03);
	}
	public function short1():Int
	{
		var value = ((this >> 8) & 0xF) - 5;
		if (5 < value) "too long LMove";
		return value;
	}
	public function short2():Int
	{
		var value = ((this >> 12) & 0xF) - 5;
		if (5 < value) "too long LMove";
		return value;
	}
	public function long():Int
	{
		var value = ((this >>  8) & 0x1F) - 15;
		if (8 < value) "too long SMove";
		return value;
	}
	public function far():Far
	{
		return new Far((this >>  8) & 0xFFFFFF);
	}
	
	// ======================
	// コマンド
	// ======================
	
	public static function _wait():Command
	{
		return new Command(0xFE);
	}
	public static function halt():Command
	{
		return new Command(0xFF);
	}
	public static function flip():Command
	{
		return new Command(0xFD);
	}
	public static function sMove(dir:Direction, long:Int):Command
	{
		return new Command(
			((long + 15) << 8) |
			(dir.toByte() << 4) |
			0x4 
		);
	}
	public static function lMove(
		dir0:Direction, short0:Int, 
		dir1:Direction, short1:Int
	):Command
	{
		return new Command(
			((short1 + 5) << 12) |
			((short0 + 5) << 8) |
			(dir1.toByte() << 6) |
			(dir0.toByte() << 4) |
			0xC
		);
	}
	
	public static function fission(
		nd:Near,
		m:Int
	):Command
	{
		return new Command(
			m << 8 |
			nd.toByte() << 3 | 
			0x5
		);
	}
	
	public static function fusionP(
		nd:Near
	):Command
	{
		return new Command(
			nd.toByte() << 3 | 
			0x7
		);
	}
	public static function fusionS(
		nd:Near
	):Command
	{
		return new Command(
			nd.toByte() << 3 | 
			0x6
		);
	}
	public static function void(
		nd:Near
	):Command
	{
		return new Command(
			nd.toByte() << 3 | 
			0x2
		);
	}
	public static function fill(
		nd:Near
	):Command
	{
		return new Command(
			nd.toByte() << 3 | 
			0x3
		);
	}
	public static function gVoid(
		nd:Near, far:Far
	):Command
	{
		return new Command(
			far.toByte() << 8 |
			nd.toByte() << 3 | 
			0x0
		);
	}
	public static function gFill(
		nd:Near, far:Far
	):Command
	{
		return new Command(
			far.toByte() << 8 |
			nd.toByte() << 3 | 
			0x1
		);
	}
}
