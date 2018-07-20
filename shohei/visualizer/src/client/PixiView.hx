package;
import core.RootContext;
import haxe.ds.Option;
import js.Browser;
import pixi.core.graphics.Graphics;
import pixi.core.sprites.Sprite;
import pixi.core.textures.Texture;
import pixi.plugins.app.Application;
import tweenxcore.Tools.FloatTools;
import tweenxcore.color.HsvColor;

class PixiView extends Application 
{
    static var WIDTH = 900;
    static var HEIGHT = 650;
    
    var _graphic:Graphics;
    var rootContext:RootContext;
    
    
	public function new(rootContext:RootContext) {
		super();
        this.rootContext = rootContext;

		position = Application.POSITION_FIXED;
		width = WIDTH;
		height = HEIGHT;
        autoResize = false;
        this.position = "relative";
		backgroundColor = 0x006666;
		transparent = true;
		antialias = false;
		super.start("auto", Browser.document.getElementById("pixi"));

		_graphic = new Graphics();
        drawBackground();
        
		stage.addChild(_graphic);
	}
    
    function drawBackground() {
        _graphic.clear();
		_graphic.beginFill(0xF9F9F9, 1);
		_graphic.drawRect(0, 0, WIDTH, HEIGHT);
		_graphic.endFill();
    }
        
    public function update():Void
    {
        drawBackground();
        
        
    }
}
