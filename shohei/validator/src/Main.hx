package;
import haxe.Int64;
import haxe.Json;
import haxe.ds.Option;
import haxe.io.BytesInput;
import sys.FileSystem;
import sys.io.File;

class Main 
{
	public static function main() 
	{
		var time = Date.now().getTime();
		
		var file = Sys.args()[0];
		var source = if (FileSystem.exists(file))
		{
			Option.Some(new BytesInput(File.getBytes(file)));
		}
		else
		{
			Option.None;
		}
		
		var file = Sys.args()[1];
		var target = if (FileSystem.exists(file))
		{
			Option.Some(new BytesInput(File.getBytes(file)));
		}
		else
		{
			Option.None;
		}
		
		try 
		{
			var traceData = new BytesInput(File.getBytes((Sys.args()[2])));
			var tracer = new Tracer(new Game(source, target), traceData);
			File.saveContent(
				Sys.args()[3],
				Json.stringify({ 
					result: "success",
					step: tracer.stepLog.length,
					energy: Int64.fromFloat(tracer.energy),
					validateTime: (Date.now().getTime() - time) + "ms",
					validateVersion: 1,
				})
			);
		}
		catch (e:Dynamic)
		{
			var message;
			if (Std.is(e, String)) 
			{
				message = e;
			}
			else
			{
				message = "unknown";
			}
			
			File.saveContent(
				Sys.args()[3],
				Json.stringify({ 
					result: "fail",
					message: message,
				})
			);
		}
	}
}
