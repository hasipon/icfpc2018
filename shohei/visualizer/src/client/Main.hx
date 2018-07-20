package;

import component.root.RootView;
import core.RootContext;
import haxe.Timer;
import js.Browser;
import js.html.KeyboardEvent;
import react.React;
import react.ReactDOM;
/**
 * ...
 * @author shohei909
 */
class Main 
{
    private static var rootPixi:PixiView;
    private static var rootContext:RootContext;
    
	static function main() 
    {
        rootContext = new RootContext();
        rootPixi = new PixiView(rootContext);
        
        rootContext.updateUi = render;
        rootContext.updatePixi = rootPixi.update;
        
        render();
        
        update();

        Browser.document.onkeydown = onKeyDown;
	}
    
    private static function update():Void
    {
        rootContext.onFrame(15);
        Timer.delay(update, 15);
    }
    
    public static function render():Void
    {
        ReactDOM.render(
            React.createElement(
                RootView,
                {
                    context: rootContext,
                }
            ),
            Browser.document.getElementById('control')
        );
    }
    
    public static function onKeyDown(e:KeyboardEvent):Bool
    {
        if (e.altKey || e.ctrlKey || e.shiftKey) { return true; } 
        return switch (e.keyCode)
        {
            case _:
                true;
        }
    }
}
