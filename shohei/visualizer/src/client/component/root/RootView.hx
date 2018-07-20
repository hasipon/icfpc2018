package component.root;
import core.RootContext;
import haxe.ds.Option;
import js.html.Event;
import react.React;
import react.ReactComponent;
import react.ReactComponent.ReactComponentOfProps;

class RootView extends ReactComponentOfProps<RootProps>
{
    public function new (props) { super(props); }
    
    override public function render():ReactElement
    {
        var i = 0;
        return "div".createElement(
            {
                className: "root",
            },
            [
                "div".createElement(
                    {},
                    []
                ),
                "div".createElement(
                    {},
                    "version : 3.3"
                ),
            ]
        );
    }
}

typedef RootProps = 
{
    context: RootContext,
}
