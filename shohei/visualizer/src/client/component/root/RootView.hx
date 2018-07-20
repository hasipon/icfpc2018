package component.root;
import core.RootContext;
import haxe.ds.Option;
import js.html.Event;
import js.html.SelectElement;
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
                    [
						"select".createElement(
							{ name: "problem", onChange: onProblemSelect },
							[for (problem in props.context.problems)
								"option".createElement(
									{ value: problem },
									[ problem ]
								)
							]
							
						)
					]
                ),
                "div".createElement(
                    {},
                    props.context.errorText
                ),
                "div".createElement(
                    {},
                    "version : 3.3"
                ),
            ]
        );
    }
	
	public function onProblemSelect(e:Event):Void
	{
		var selectElement:SelectElement = cast e.target;
		props.context.selectProblem(props.context.problems[selectElement.selectedIndex]);
	}
}

typedef RootProps = 
{
    context: RootContext,
}
