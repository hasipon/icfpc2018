package;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import haxe.io.BytesOutput;
import haxe.zip.Compress;
import haxe.zip.Uncompress;

class GZip 
{
	public static function unzip(bytes:Bytes):Bytes
	{
		var input = new BytesInput(bytes);
		input.bigEndian = false;
		input.readByte(); input.readByte();
		return Uncompress.run(input.read(input.length - input.position - 4));
	}
	
	public static function zip(bytes:Bytes):Bytes
	{
		var output = new BytesOutput();
		output.writeByte(0x1F);
		output.writeByte(0x8B);
		output.write(Compress.run(bytes, 1));
		return output.getBytes();
	}
}