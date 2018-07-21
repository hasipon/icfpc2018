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
	private var name:String;
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
	
    public function new()
    {
        hash = null;
		
		problemNumber = Std.parseInt(Resource.getString("size"));
		problems = [for (i in 1...problemNumber + 1) "LA" + StringTools.lpad("" + i, "0", 3)];
		game = Option.None;
		tracer = Option.None;
		loading = false;
		playing = true;
		speed = "1";
		targetDir = "submission/nbt";
		
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
				var data = if (hash != "") Json.parse(StringTools.urlDecode(hash)) else {dir:"submission/nbt", model:"LA001"};
				changeTargetDir(data.dir);
				selectProblem(data.model);
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
			
			loading = true;
			updateHash();
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
	public function changeTargetDir(targetDir:String):Void
	{
		if (this.targetDir != targetDir)
		{
			this.targetDir = targetDir;
			updateUi();
			updateHash();
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
	
    private function updateHash():Void 
    {
        this.hash = Json.stringify(
			{
				"model": name,
				"dir": targetDir,
			}
		);
        Browser.location.hash = "#" + this.hash;
    }
}
