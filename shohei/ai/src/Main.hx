package;
import haxe.ds.Option;
import haxe.io.BytesInput;
import sys.FileSystem;
import sys.io.File;

class Main 
{
	public static function main() 
	{
		var file = Sys.args()[0];
		var target = if (FileSystem.exists(file))
		{
			Option.Some(new BytesInput(File.getBytes(file)));
		}
		else
		{
			Option.None;
		}
		var file = Sys.args()[1];
		var source = if (FileSystem.exists(file))
		{
			Option.Some(new BytesInput(File.getBytes(file)));
		}
		else
		{
			Option.None;
		}
		
		var bytes = new Ai(source, target).execute();
		trace(Sys.args()[2], bytes);
		
		File.saveBytes(Sys.args()[2], bytes);
	}
}
