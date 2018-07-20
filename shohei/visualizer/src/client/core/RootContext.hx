package core;
import haxe.Http;
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

class RootContext 
{
    private var hash:String;
	public var errorText:String;
    public var updateUi:Void->Void;
    public var updateGraphic:Void->Void;
	
	public var game:Option<Game>;
    public var problemNumber:Int;
	public var problems:Array<String>;
	public var loading:Bool;
	
    public function new()
    {
        hash = "";
		
		problemNumber = Std.parseInt(Resource.getString("size"));
		problems = [for (i in 1...problemNumber + 1) StringTools.lpad("" + i, "0", 3)];
		game = Option.None;
		loading = false;
    }
    
    public function onFrame(ms:Float):Void
    {
        var hash = Browser.location.hash.substr(1);
        if (this.hash != hash)
        {
            updateHash(hash);
        }
    }
    
	public function selectProblem(name:String):Void
	{
		game = Option.None;
		loading = true;
		updateUi();
		updateGraphic();
		
		var xhr = new XMLHttpRequest();
		var file = "/problemsL/LA" + name + "_tgt.mdl";
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
	
	
	public function loadedProbrem(name:String, byteInput:BytesInput):Void
	{
		game = Option.Some(new Game(byteInput));
		loading = false;
		updateUi();
		updateGraphic();
	}
	
    private function updateHash(hash:String) 
    {
        this.hash = hash;
        var http = new Http(hash);
        http.onData = function (data)
        {
			trace(data);
        }
        http.request();
    }
}
