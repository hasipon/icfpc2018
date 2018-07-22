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
								tracer.index + "/" + (tracer.stepLog.length) + "ステップ"
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
					switch (props.context.game)
					{
						case Option.Some(game):
							([
								"サイズ(R):" + game.size,
								"br".createElement({}),
							]:Array<Dynamic>).concat(
								switch (game.sourceModelInput)
								{
									case Option.Some(_):
										[
											"ソースのバウンド",
											" X:" + (game.sourceMaxX - game.sourceMinX + 1),
											" Y:" + (game.sourceMaxY - game.sourceMinY + 1),
											" Z:" + (game.sourceMaxZ - game.sourceMinZ + 1),
											"br".createElement({}),
										];
										
									case Option.None:
										[];
								}
							).concat(
								switch (game.targetModelInput)
								{
									case Option.Some(_):
										[
											"ターゲットのバウンド",
											" X:" + (- game.targetMinX + game.targetMaxX + 1),
											" Y:" + (- game.targetMinY + game.targetMaxY + 1),
											" Z:" + (- game.targetMinZ + game.targetMaxZ + 1),
											"br".createElement({}),
										];
										
									case Option.None:
										[];
								}
							);
							
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
								"エナジー:" + tracer.game.energy,
								"br".createElement({}),
								"ハーモニクス:" + if (tracer.game.highHarmonics) "High" else "Low",
								"br".createElement({}),
							];
							
						case Option.None:
							[];
					}
				),
				"pre".createElement(
                    {},
					switch (props.context.tracer)
					{
						case Option.Some(tracer):
							[
								for (bot in tracer.game.bots)
								{
									if (bot.isActive)
									{
										"ボット" + (bot.id + 1)+ ":" + bot.seeds.join(",") + "\n";
									}
								}
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
									{ value: problem, selected: props.context.name == problem },
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
							"のディレクトリでトレース開始"
						),
						"br".createElement({}),
						"input".createElement(
							{ type : "text", value : props.context.targetFile, onChange: onChangeTargetFile }
						),
						"button".createElement(
							{ name: "targetTrace", onClick:onFileTraceClick },
							"のファイルでトレース開始"
						),
						"br".createElement({}),
						"input".createElement(
							{ 
								type:"file", 
								accept:".gz", 
								onChange:onChangeUpfile
							}
						),
					]
                ),
                "div".createElement(
                    {},
					[
						"button".createElement(
							{ name: "targetTrace", onClick:onTurnLeftClick },
							"<<"
						),
						"左右回転:" + (props.context.rot * 90) + "°",
						"button".createElement(
							{ name: "targetTrace", onClick:onTurnRightClick },
							">>"
						)
					]
                ),
                "div".createElement(
                    {},
					[
						"上下回転:" ,
						"input".createElement(
							{ 
								type : "range",
								value : props.context.cameraAngle,
								min : 0,
								max : 1,
								onChange: onCameraAngleChange,
								step: 0.01,
								style: {width: "400px"}
							}
						),
						props.context.cameraAngle
					]
                ),
                "div".createElement(
                    {},
                    props.context.errorText
                ),
                "div".createElement(
                    {},
                    "version : 13"
                ),
            ]
        );
    }
	
	public function onProblemSelect(e:Event):Void
	{
		var selectElement:SelectElement = cast e.target;
		props.context.selectProblem(props.context.problems[selectElement.selectedIndex]);
		props.context.updateHash();
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
	public function onFileTraceClick():Void
	{
		props.context.startFileTrace();
	}
	public function onChangeTargetDir(e:Event):Void
	{
		var input:InputElement = cast e.target;
		props.context.changeTargetDir(input.value);
		props.context.updateHash();
	}
	public function onChangeTargetFile(e:Event):Void
	{
		var input:InputElement = cast e.target;
		props.context.changeTargetFile(input.value);
		props.context.updateHash();
	}
	public function onSpeedChange(e:Event):Void
	{
		var range:InputElement = cast e.target;
		props.context.changeSpeed(range.value);
	}
	public function onTurnLeftClick(e:Event):Void
	{
		props.context.turn(1);
	}
	public function onTurnRightClick(e:Event):Void
	{
		props.context.turn(3);
	}
	public function onCameraAngleChange(e:Event):Void
	{
		var range:InputElement = cast e.target;
		props.context.changeCameraAngle(Std.parseFloat(range.value));
	}
	public function onChangeUpfile(e:Event):Void
	{
		var input:InputElement = cast e.target;
		props.context.changeUpfile(input.files[0]);
	}
}

typedef RootProps = 
{
    context: RootContext,
}
