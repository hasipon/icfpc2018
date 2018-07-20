package core;
import haxe.Http;
import js.Browser;

class RootContext 
{
    private var hash:String;
    public var updateUi:Void->Void;
    public var updatePixi:Void->Void;
    
    public function new()
    {
        hash = "";
    }
    
    public function onFrame(ms:Float):Void
    {
        var hash = Browser.location.hash.substr(1);
        if (this.hash != hash)
        {
            updateHash(hash);
        }
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
