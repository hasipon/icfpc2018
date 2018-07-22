package;
import haxe.io.Bytes;
import haxe.io.BytesInput;
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
}