package;
import haxe.ds.Vector;
import haxe.io.BytesInput;

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
		return ((this >>  8) & 0xF) - 5;
	}
	public function short2():Int
	{
		return ((this >> 12) & 0xF) - 5;
	}
	public function long():Int
	{
		return ((this >>  8) & 0x1F) - 15;
	}
	public function far():Far
	{
		return new Far((this >>  8) & 0xFFFFFF);
	}
}
