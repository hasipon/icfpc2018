package core;
import haxe.Http;
import haxe.Json;
import haxe.Resource;
import haxe.ds.Option;
import haxe.io.Bytes;
import haxe.io.BytesInput;
import js.Browser;
import js.html.ArrayBuffer;
import js.html.Event;
import js.html.Uint8Array;
import js.html.XMLHttpRequest;
import js.html.XMLHttpRequestResponseType;
import pixi.plugins.spine.core.TrackEntry;

class RootContext 
{
    private var hash:String;
	public var name:String;
	public var errorText:String;
    public var updateUi:Void->Void;
    public var updateGraphic:Void->Void;
	
	public var game:Option<Game>;
	public var tracer:Option<Tracer>;
    public var problemNumber:Int;
	public var problems:Array<String>;
	public var loading:Bool;
	public var startable(get, never):Bool;
	private function get_startable():Bool 
	{
		return !loading && game.match(Option.Some(_));
	}
	
	public var playing:Bool;
	public var speed:String;
	public var targetDir:String;
	public var targetFile:String;
	public var rot:Int;
	public var cameraAngle:Float;
	
    public function new()
    {
		cameraAngle = 0.5;
        hash = null;
		
		problemNumber = Std.parseInt(Resource.getString("size"));
		problems = [for (i in 1...problemNumber + 1) "LA" + StringTools.lpad("" + i, "0", 3)];
		game = Option.None;
		tracer = Option.None;
		loading = false;
		playing = true;
		speed = "1";
		targetDir = "";
		targetFile = "";
		rot = 0;
		name = "";
    }
    
    public function onFrame(ms:Float):Void
    {
        var hash = Browser.location.hash.substr(1);
        if (this.hash != hash)
        {
			this.hash = hash;
			
			try
			{
				var data:Dynamic = if (hash != "") Json.parse(StringTools.urlDecode(hash)) else {};
				if (data.dir == null) data.dir = "submission/nbt";
				if (data.model == null) data.model = "LA001";
				if (data.file == null) data.file = "submission/nbt/" + data.model + ".nbt";
				
				changeTargetDir(data.dir);
				changeTargetFile(data.file);
				selectProblem(data.model);
				updateHash();
			}
			catch (e:Dynamic)
			{
				errorText = "エラー: " + e;
			}
        }
		
		if (playing)
		{
			switch (tracer)
			{
				case Option.Some(tracer):
					var prevIndex = tracer.index;
					tracer.move(Std.parseFloat(speed));
					if (prevIndex != tracer.index)
					{
						updateUi();
						updateGraphic();
					}
					
				case Option.None:
			}
		}
    }
    
	public function selectProblem(name:String):Void
	{
		if (this.name != name)
		{
			this.name = name;
			tracer = Option.None;
			game = Option.None;
			targetFile = targetDir + "/" + name + ".nbt";
			
			loading = true;
			updateUi();
			updateGraphic();
			
			var xhr = new XMLHttpRequest();
			var file = "../../../problemsL/" + name + "_tgt.mdl";
			xhr.open('GET', file, true);
			xhr.responseType = XMLHttpRequestResponseType.ARRAYBUFFER;

			xhr.onload = function(e:Event) {
				var arrayBuffer:ArrayBuffer = xhr.response;
				loadedProbrem(name, new BytesInput(Bytes.ofData(arrayBuffer)));
			};
			
			xhr.onerror  = function(e:Event) {
				loading = false;
				errorText = "エラー: ファイル読み込みエラー:" + file;
				
				updateUi();
				updateGraphic();
			};
			
			xhr.send();
		}
	}
	
	public function startDefaultTrace():Void
	{
		startTrace("../../../dfltTracesL/" + name + ".nbt");
	}
	public function startTargetTrace():Void
	{
		startTrace("../../../" + targetDir + "/" + name + ".nbt");
	}
	public function startFileTrace():Void
	{
		startTrace("../../../" + targetFile);
	}
	public function changeTargetDir(targetDir:String):Void
	{
		if (this.targetDir != targetDir)
		{
			this.targetDir = targetDir;
			updateUi();
		}
	}
	public function changeTargetFile(targetFile:String):Void
	{
		if (this.targetFile != targetFile)
		{
			trace(targetFile);
			this.targetFile = targetFile;
			updateUi();
		}
	}
	
	private function startTrace(file:String):Void
	{
		var xhr = new XMLHttpRequest();
		
		tracer = Option.None;
		loading = true;
		updateUi();
		updateGraphic();
		
		xhr.open('GET', file, true);
		xhr.responseType = XMLHttpRequestResponseType.ARRAYBUFFER;

		xhr.onload = function(e:Event) {
			var arrayBuffer:ArrayBuffer = xhr.response;
			loadedTrace(new BytesInput(Bytes.ofData(arrayBuffer)));
		};
		
		xhr.onerror  = function(e:Event) {
			loading = false;
			errorText = "エラー: ファイル読み込みエラー:" + file;
			
			updateUi();
			updateGraphic();
		};
		
		xhr.send();
	}
	
	public function loadedProbrem(name:String, byteInput:BytesInput):Void
	{
		game = Option.Some(new Game(byteInput));
		loading = false;
		updateUi();
		updateGraphic();
	}
	
	public function loadedTrace(byteInput:BytesInput):Void
	{
		loading = false;
		switch (game)
		{
			case Option.Some(game):
				tracer = Option.Some(new Tracer(game, byteInput));
				
			case Option.None:
		}
		
		updateUi();
		updateGraphic();
	}
	
	public function gotoTrace(int:Int):Void
	{
		switch (tracer)
		{
			case Option.Some(tracer):
				tracer.goto(int);
				
			case Option.None:
		}
		
		updateUi();
		updateGraphic();
	}
	
	public function togglePlaying():Void
	{
		playing = !playing;
		updateUi();
		updateGraphic();
	}
	
	public function changeSpeed(speed:String):Void
	{
		this.speed = speed;
		updateUi();
		updateGraphic();
	}
	
	
	public function turn(i:Int):Void
	{
		this.rot = (this.rot + i) % 4;
		updateUi();
		updateGraphic();
	}
	
	public function changeCameraAngle(cameraAngle:Float):Void
	{
		this.cameraAngle = cameraAngle;
		updateUi();
		updateGraphic();
	}
	
    public function updateHash():Void 
    {
    }
}
