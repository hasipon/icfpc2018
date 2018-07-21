package component.root;
import core.RootContext;
import haxe.ds.Option;
import js.html.Event;
import js.html.InputElement;
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
					switch (props.context.tracer)
					{
						case Option.Some(tracer):
							[
								"コマンド：",
								"input".createElement(
									{ 
										type : "range",
										value :  tracer.index,
										min : 0,
										max : tracer.stepLog.length - 1,
										onChange: onRangeChange,
										style: {width: "800px"}
									}
								),
								tracer.index + "/" + (tracer.stepLog.length)
							];
							
						case Option.None:
							[];
					}
				),
				"div".createElement(
                    {},
					switch (props.context.tracer)
					{
						case Option.Some(tracer):
							[
								"button".createElement(
									{ name: "defaultTrace", onClick:onPlayClick },
									if (props.context.playing) "停止" else "再生"
								)
							];
							
						case Option.None:
							[];
					}
				),
				"div".createElement(
                    {},
					switch (props.context.tracer)
					{
						case Option.Some(tracer):
							[
								"再生速度",
								"input".createElement(
									{ 
										type : "range",
										value : props.context.speed,
										min : -200,
										max : 200,
										onChange: onSpeedChange,
										step: 0.01,
										style: {width: "400px"}
									}
								),
								"input".createElement(
									{ type : "text", value : props.context.speed, onChange: onSpeedChange }
								),
							];
							
						case Option.None:
							[];
					}
				),
				"hr".createElement({}),
				
				"div".createElement(
                    {},
					switch (props.context.tracer)
					{
						case Option.Some(tracer):
							[
								"エナジー:" + tracer.game.energy + "くらい",
								"br".createElement({}),
								"ステップ:" + tracer.game.step,
								"br".createElement({}),
								"ハーモニクス:" + if (tracer.game.highHarmonics) "High" else "Low",
								"br".createElement({}),
							];
							
						case Option.None:
							[];
					}
				),
				"hr".createElement({}),
                "div".createElement(
                    {},
                    [
						"select".createElement(
							{ name: "problem", onChange: onProblemSelect, disabled: props.context.loading },
							[for (problem in props.context.problems)
								"option".createElement(
									{ value: problem },
									[ problem ]
								)
							]
							
						),
						"br".createElement({}),
						"button".createElement(
							{ name: "defaultTrace", onClick:onDefaultTraceClick, disabled: !props.context.startable },
							"デフォルトトレース開始"
						),
						"br".createElement({}),
						"input".createElement(
							{ type : "text", value : props.context.targetDir, onChange: onChangeTargetDir }
						),
						"button".createElement(
							{ name: "targetTrace", onClick:onTargetTraceClick },
							"のトレース開始"
						)
					]
                ),
                "div".createElement(
                    {},
                    props.context.errorText
                ),
                "div".createElement(
                    {},
                    "version : 12"
                ),
            ]
        );
    }
	
	public function onProblemSelect(e:Event):Void
	{
		var selectElement:SelectElement = cast e.target;
		props.context.selectProblem(props.context.problems[selectElement.selectedIndex]);
	}
	
	public function onDefaultTraceClick(e:Event):Void
	{
		props.context.startDefaultTrace();
	}
	public function onRangeChange(e:Event):Void
	{
		var range:InputElement = cast e.target;
		props.context.gotoTrace(Std.int(Std.parseFloat(range.value)));
	}
	public function onPlayClick():Void
	{
		props.context.togglePlaying();
	}
	public function onTargetTraceClick():Void
	{
		props.context.startTargetTrace();
	}
	public function onChangeTargetDir(e:Event):Void
	{
		var input:InputElement = cast e.target;
		props.context.changeTargetDir(input.value);
	}
	public function onSpeedChange(e:Event):Void
	{
		var range:InputElement = cast e.target;
		props.context.changeSpeed(range.value);
	}
}

typedef RootProps = 
{
    context: RootContext,
}
