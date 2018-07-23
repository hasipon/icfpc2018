package;
import haxe.Json;
import haxe.io.Bytes;
import haxe.macro.Context;
import sys.FileSystem;
import sys.io.File;

class MacroMain 
{
    public static function main():Void
    {	
		var array = [];
		for (file in FileSystem.readDirectory("../../out/"))
		{
			if (FileSystem.isDirectory("../../out/" + file))
			{
				array.push(file);
			}
		}
		
		Context.addResource("out", Bytes.ofString(Json.stringify(array)));
	}
}
