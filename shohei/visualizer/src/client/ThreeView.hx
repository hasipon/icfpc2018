package;
import core.RootContext;
import haxe.ds.Option;
import js.Browser;
import js.Three;
import js.three.AmbientLight;
import js.three.BoxGeometry;
import js.three.Color;
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
	private var bots:Array<Mesh>;
	
	public function new(rootContext:RootContext) 
	{
		this.rootContext = rootContext;
		var w = 900;
		var h = 750;
		scene = new Scene();
		
		cubes = [];
		activeCubes = 0;
		
		var geometry = new PlaneGeometry(600, 600, 1, 1);
		var material = new MeshLambertMaterial({ color:0x991111 });
		var plane = new Mesh(geometry, material);
		material.opacity = 0.3;
		material.transparent = true;
		plane.renderOrder += 1000;
		plane.position.set(0, -300, 0);
		plane.rotateOnAxis(new Vector3(1, 0, 0), -Math.PI / 2);
		scene.add(plane);
		
		var pointLight = new PointLight(0x777777, 1, 1000000, 2);
		pointLight.position.set(0, 0, 200);
		scene.add(pointLight);
		
		var pointLight = new PointLight(0x777777, 1, 1000000, 2);
		pointLight.position.set(0, 100, 400);
		scene.add(pointLight);
		
		camera = new PerspectiveCamera(70, w / h, 1, 3000);
		camera.position.z = 750;
		camera.position.y = 400;
		camera.lookAt(new Vector3(0, 0, 0));
		scene.add(camera);
		
		renderer = new WebGLRenderer();
		renderer.setSize(w, h);
		
		var light = new AmbientLight(0x666666);
		scene.add(light);
		
		Browser.document.getElementById("three").appendChild(renderer.domElement);
		bots = [
			for (i in 0...Bot.MAX)
			{
				var geometry = new PlaneGeometry(600, 600, 1, 1);
				var material = new MeshLambertMaterial({ color:0xEF9911 });
				var mesh = new Mesh(geometry, material);	
				material.opacity  = 0.5;
				material.transparent = true;
				mesh.visible = false;
				scene.add(mesh);
				mesh;
			}
		];
		update();
	}
	
	public function update():Void
	{
		var angle = rootContext.cameraAngle * Math.PI * 0.499 + 0.00001;
		camera.position.z = Math.sin(angle) * 800;
		camera.position.y = Math.cos(angle) * 800;
		camera.lookAt(new Vector3(0, 0, 0));
		
		switch (rootContext.game)
		{
			case Option.Some(game):
				var count = 0;
				var size = game.size;
				for (i in 0...20)
				{
					var logic = game.bots[i];
					var view = bots[i];
					var pos = logic.position;
					if (logic.isActive)
					{
						var rotatedX, rotatedZ;
						switch (rootContext.rot)
						{
							case 0:
								rotatedX = pos.x;
								rotatedZ = pos.z;
								
							case 1:
								rotatedX = size - pos.z - 1;
								rotatedZ = pos.x;
				
							case 2:
								rotatedX = size - pos.x - 1;
								rotatedZ = size - pos.z - 1;
								
							case 3:
								rotatedX = pos.z;
								rotatedZ = size - pos.x - 1;
								
							case _: throw "unknown rot";
							
						}
						view.position.set(
							rotatedX * 600 / size - 300,
							pos.y * 600 / size - 300,
							rotatedZ * 600 / size - 300
						);
						var scale = 1 / size * 0.5;
						view.scale.set(scale, scale, scale);
						view.visible = true;
					}
					else
					{
						view.visible = false;
					}
				}
				
				for (z in 0...size)
				{
					for (y in 0...size)
					{
						var successY = null;
						var successZ = null;
						var failY = null;
						var failZ = null;
						var targetY = null;
						var targetZ = null;
						
						for (x in 0...size)
						{
							var targetValue = getTarget(game, x, y, z);
							var currentValue = getCurrent(game, x, y, z);
							
							if (targetValue && currentValue) // success
							{
								var nextZ = z + 1;
								if (nextZ == size || !getCurrent(game, x, y, nextZ) || !getTarget(game, x, y, nextZ))
								{
									if (successZ == null)
									{
										var cube = getCube(count);
										setPlaneZ(cube, x, y, z, size);
										setMaterial(cast cube.material, 0x1155DD, 0.1);
										
										successZ = cube;
										count++;
									}
									else
									{
										successZ.position.x += 1 / size / 2 * 600;
										successZ.scale.x += 1 / size;
									}
								}
								else
								{
									successZ = null;
								}
								
								var nextY = y + 1;
								if (nextY == size || !getCurrent(game, x, nextY, z) || !getTarget(game, x, nextY, z))
								{
									if (successY == null)
									{
										var cube = getCube(count);
										setPlaneY(cube, x, y, z, size);
										setMaterial(cast cube.material, 0x1155DD, 0.1);
										
										successY = cube;
										count++;
									}
									else
									{
										successY.position.x += 1 / size / 2 * 600;
										successY.scale.x += 1 / size;
									}
								}
								else
								{
									successY = null;
								}
								
								failY = null;
								failZ = null;
								targetY = null;
								targetZ = null;
							}
							else if (!targetValue && currentValue) // fail
							{
								var nextZ = z + 1;
								if (nextZ == size || !getCurrent(game, x, y, nextZ) || getTarget(game, x, y, nextZ))
								{
									if (failZ == null)
									{
										var cube = getCube(count);
										setPlaneZ(cube, x, y, z, size);
										setMaterial(cast cube.material, 0xDD5555, 0.1);
										
										failZ = cube;
										count++;
									}
									else
									{
										failZ.position.x += 1 / size / 2 * 600;
										failZ.scale.x += 1 / size;
									}
								}
								else
								{
									failZ = null;
								}
								
								var nextY = y + 1;
								if (nextY == size || !getCurrent(game, x, nextY, z) || getTarget(game, x, nextY, z))
								{
									if (failY == null)
									{
										var cube = getCube(count);
										setPlaneY(cube, x, y, z, size);
										setMaterial(cast cube.material, 0xDD5555, 0.1);
										
										failY = cube;
										count++;
									}
									else
									{
										failY.position.x += 1 / size / 2 * 600;
										failY.scale.x += 1 / size;
									}
								}
								else
								{
									failY = null;
								}
								
								successY = null;
								successZ = null;
								targetY = null;
								targetZ = null;
							}
							else if (targetValue && !currentValue) // target
							{
								var nextZ = z + 1;
								if (nextZ == size || !getTarget(game, x, y, nextZ) || getCurrent(game, x, y, nextZ))
								{
									if (targetZ == null)
									{
										var cube = getCube(count);
										setPlaneZ(cube, x, y, z, size);
										setMaterial(cast cube.material, 0x11DD55, 0.1);
										targetZ = cube;
										count++;
									}
									else
									{
										targetZ.position.x += 1 / size / 2 * 600;
										targetZ.scale.x += 1 / size;
									}
								}
								else
								{
									targetZ = null;
								}
								var nextY = y + 1;
								if (nextY == size || !getTarget(game, x, nextY, z) || getCurrent(game, x, nextY, z))
								{
									if (targetY == null)
									{
										var cube = getCube(count);
										setPlaneY(cube, x, y, z, size);
										setMaterial(cast cube.material, 0x11DD55, 0.1);
										
										targetY = cube;
										count++;
									}
									else
									{
										targetY.position.x += 1 / size / 2 * 600;
										targetY.scale.x += 1 / size;
									}
								}
								else
								{
									targetY = null;
								}
								
								failY = null;
								failZ = null;
								successY = null;
								successZ = null;
							}
							else
							{
								successY = null;
								successZ = null;
								failY = null;
								failZ = null;
								targetY = null;
								targetZ = null;
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
	
	private function getCurrent(game:Game, x, y, z):Bool
	{
		var size = game.size;
		return switch (rootContext.rot)
		{
			case 0:
				game.currentModel[x][y][z];
					
			case 1:
				game.currentModel[z][y][size - x - 1];
				
			case 2:
				game.currentModel[size - x - 1][y][size - z - 1];
				
			case 3:
				game.currentModel[size - z - 1][y][x];
				
			case _: throw "unknown rot";
		}
	}
	private function getTarget(game:Game, x, y, z):Bool
	{
		var size = game.size;
		return switch (rootContext.rot)
		{
			case 0:
				game.targetModel[x][y][z];
					
			case 1:
				game.targetModel[z][y][size - x - 1];
				
			case 2:
				game.targetModel[size - x - 1][y][size - z - 1];
				
			case 3:
				game.targetModel[size - z - 1][y][x];
				
			case _: throw "unknown rot";
		}
	}
	
	function getCube(index:Int):Mesh
	{
		if (cubes.length <= index)
		{
			var geometry = new PlaneGeometry(600, 600, 1, 1);
			var material = new MeshLambertMaterial({ color:0x11DD55 });
			var cube = new Mesh(geometry, material);
			cube.castShadow = true;
			cube.receiveShadow = true;
			cubes.push(cube);
			scene.add(cube);
		}
		
		return cubes[index];
	}
	
	function setPlaneY(cube:Mesh, x:Int, y:Int, z:Int, size:Int):Void
	{
		cube.position.set(
			x * 600 / size - 300,
			(y + 0.5) * 600 / size - 300,
			z * 600 / size - 300
		);
		var scale = 1 / size;
		cube.scale.set(scale * 0.95, scale * 0.95, scale);
		cube.rotation.set( -Math.PI / 2, 0, 0);
		cube.visible = true;
	}
	function setPlaneZ(cube:Mesh, x:Int, y:Int, z:Int, size:Int):Void
	{
		cube.position.set(
			x * 600 / size - 300,
			y * 600 / size - 300,
			(z + 0.5) * 600 / size - 300
		);
		var scale = 1 / size;
		cube.scale.set(scale * 0.95, scale * 0.95, scale);
		cube.rotation.set(0, 0, 0);
		cube.visible = true;
	}
	public function setMaterial(material:MeshLambertMaterial, color:Int, alpha:Float):Void
	{
		material.color.setHex(color);
		material.opacity = alpha;
		material.transparent = true;
	}

	function setActiveCount(count:Int):Void
	{
		for (i in count...activeCubes)
		{
			cubes[i].visible = false;
		}
		activeCubes = count;
	}
}