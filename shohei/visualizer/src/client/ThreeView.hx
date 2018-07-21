package;
import core.RootContext;
import haxe.ds.Option;
import js.Browser;
import js.Three;
import js.three.AmbientLight;
import js.three.BoxGeometry;
import js.three.CubeGeometry;
import js.three.Mesh;
import js.three.MeshLambertMaterial;
import js.three.MeshStandardMaterial;
import js.three.PerspectiveCamera;
import js.three.Plane;
import js.three.PlaneGeometry;
import js.three.PointLight;
import js.three.Scene;
import js.three.Vector3;
import js.three.WebGLRenderer;

class ThreeView 
{
	private var rootContext:RootContext;
	
	private var camera:PerspectiveCamera;
	private var renderer:WebGLRenderer;
	private var scene:Scene;

	private var cubes:Array<Mesh>;
	private var activeCubes:Int;
	
	public function new(rootContext:RootContext) 
	{
		this.rootContext = rootContext;
		var w = 900;
		var h = 750;
		scene = new Scene();
		
		cubes = [];
		activeCubes = 0;
		
		var geometry = new PlaneGeometry(600, 600, 20, 20);
		var material = new MeshLambertMaterial({ color:0x771111 });
		var plane = new Mesh(geometry, material);
		plane.renderOrder += 1000;
		plane.position.set(0, -300, 0);
		plane.rotateOnAxis(new Vector3(1, 0, 0), -Math.PI / 2);
		plane.receiveShadow = true;
		scene.add(plane);
		
		var pointLight = new PointLight(0x777777, 1, 1000000, 2);
		pointLight.position.set(0, 0, 200);
		pointLight.castShadow = true;
		pointLight.shadowMapWidth = 2048;
		pointLight.shadowMapHeight = 2048; 
		scene.add(pointLight);
		
		var pointLight = new PointLight(0x777777, 1, 1000000, 2);
		pointLight.position.set(0, 100, 400);
		pointLight.castShadow = true;
		pointLight.shadowMapWidth = 2048;
		pointLight.shadowMapHeight = 2048; 
		scene.add(pointLight);
		
		camera = new PerspectiveCamera(70, w / h, 1, 1000);
		camera.position.z = 750;
		scene.add(camera);
		
		renderer = new WebGLRenderer();
		renderer.setSize(w, h);
		renderer.shadowMapEnabled = true;
		
		var light = new AmbientLight(0x666666);
		scene.add(light);
		
		Browser.document.getElementById("three").appendChild(renderer.domElement);
		
		update();
	}
	
	public function update():Void
	{
		switch (rootContext.game)
		{
			case Option.Some(game):
				var count = 0;
				var size = game.size;
				for (x in 0...size)
				{
					for (y in 0...size)
					{
						for (z in 0...size)
						{
							if (game.currentModel[x][y][z])
							{
								var cube = getCube(count);
								cube.position.set(
									x * 600 / size - 300,
									y * 600 / size - 300,
									z * 600 / size - 300
								);
								var scale = 1 / size * 0.95;
								cube.scale.set(scale, scale, scale);
								var material:MeshLambertMaterial = cast cube.material;
								material.opacity  = 1;
								material.transparent = false;
								cube.visible = true;
								cube.receiveShadow = true;
								cube.castShadow = true;
								count++;
							}
							else if (game.targetModel[x][y][z])
							{
								var cube = getCube(count);
								cube.position.set(
									x * 600 / size - 300,
									y * 600 / size - 300,
									z * 600 / size - 300
								);
								var scale = 1 / size * 0.95;
								cube.scale.set(scale, scale, scale);
								var material:MeshLambertMaterial = cast cube.material;
								material.opacity = 0.1;
								material.transparent = true;
								cube.visible = true;
								cube.receiveShadow = false;
								cube.castShadow = false;
								count++;
							}
						}
					}
				}
				setActiveCount(count);
				
			case Option.None:
				setActiveCount(0);
		}
		renderer.render(scene, camera);
	}
	
	function getCube(index:Int):Mesh
	{
		if (cubes.length <= index)
		{
			var geometry = new CubeGeometry(600, 600, 600, 1, 1, 1);
			var material = new MeshLambertMaterial({ color:0x11DD55 });
			var cube = new Mesh(geometry, material);
			cube.castShadow = true;
			cube.receiveShadow = true;
			cubes.push(cube);
			scene.add(cube);
		}
		
		return cubes[index];
	}
	
	function setActiveCount(count:Int):Void
	{
		trace(count);
		for (i in count...activeCubes)
		{
			cubes[i].visible = false;
		}
		activeCubes = count;
	}
}