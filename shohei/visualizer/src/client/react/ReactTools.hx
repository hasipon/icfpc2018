package react;
import react.React;
import react.ReactComponent.ReactComponentOfProps;
import react.ReactComponent.ReactElement;

class ReactTools 
{
    public static function createElement<TProps>(type:Class<ReactComponentOfProps<TProps>>, ?attrs:TProps):ReactElement
    {
        return cast React.createElement(type, attrs);
    }
}

class ReactStringTools 
{
    public static function createElement(type:String, attrs:Dynamic, ?children0:Dynamic):ReactElement
    {
        return cast React.createElement(type, attrs, children0);
    }
}
