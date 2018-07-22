package;
import haxe.io.BytesInput;
import sys.io.File;

class Main 
{
	public static function main() 
	{
		var input = new BytesInput(File.getBytes(Sys.args()[0]));
		
		var bytes = new Ai(input).execute();
		File.saveBytes(Sys.args()[1], bytes);
	}
}
