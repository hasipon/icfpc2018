package;
import haxe.io.Bytes;
import haxe.macro.Context;
import sys.FileSystem;
import sys.io.File;

class MacroMain 
{
    public static function main():Void
    {	
		var size = 0;
		var dir = "../../problemsL";
        for (file in FileSystem.readDirectory(dir))
        {
            if (StringTools.endsWith(file, ".mdl"))
            {
				size++;
            }
        }
		
		Context.addResource("size", Bytes.ofString("" + size));
	}
}
