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
		camera.rotation.set(-Math.PI / 5, 0, 0);
		scene.add(camera);
		
		renderer = new WebGLRenderer();
		renderer.setSize(w, h);
		
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
				
				for (z in 0...size)
				{
					for (y in 0...size)
					{
						var currentY = null;
						var currentZ = null;
						var targetY = null;
						var targetZ = null;
						
						for (x in 0...size)
						{
							if (game.currentModel[x][y][z])
							{
								var nextZ = z + 1;
								if (nextZ == size || !game.currentModel[x][y][nextZ])
								{
									if (currentZ == null)
									{
										var cube = getCube(count);
										cube.position.set(
											x * 600 / size - 300,
											y * 600 / size - 300,
											(z + 0.5) * 600 / size - 300
										);
										var scale = 1 / size;
										cube.scale.set(scale * 0.95, scale * 0.95, scale);
										var material:MeshLambertMaterial = cast cube.material;
										material.color.setHex(0x1155DD);
										material.opacity  = 0.4;
										material.transparent = true;
										cube.visible = true;
										cube.receiveShadow = false;
										cube.castShadow = false;
										cube.rotation.set(0, 0, 0);
										currentZ = cube;
										count++;
									}
									else
									{
										currentZ.position.x += 1 / size / 2 * 600;
										currentZ.scale.x += 1 / size;
									}
								}
								else
								{
									currentZ = null;
								}
								
								var nextY = y + 1;
								if (nextY == size || (!game.targetModel[x][nextY][z] && !game.currentModel[x][nextY][z]))
								{
									if (currentY == null)
									{
										var cube = getCube(count);
										cube.position.set(
											x * 600 / size - 300,
											(y + 0.5) * 600 / size - 300,
											z * 600 / size - 300
										);
										var scale = 1 / size;
										cube.scale.set(scale * 0.95, scale * 0.95, scale);
										var material:MeshLambertMaterial = cast cube.material;
										material.color.setHex(0x1155DD);
										material.opacity  = 0.4;
										material.transparent = true;
										cube.visible = true;
										cube.receiveShadow = true;
										cube.castShadow = true;
										cube.rotation.set( -Math.PI / 2, 0, 0);
										
										currentY = cube;
										count++;
									}
									else
									{
										currentY.position.x += 1 / size / 2 * 600;
										currentY.scale.x += 1 / size;
									}
								}
								else
								{
									currentY = null;
								}
								targetY = null;
								targetZ = null;
							}
							else if (game.targetModel[x][y][z])
							{
								var nextZ = z + 1;
								if (nextZ == size || (!game.targetModel[x][y][nextZ] && !game.currentModel[x][y][nextZ]))
								{
									if (targetZ == null)
									{
										var cube = getCube(count);
										cube.position.set(
											x * 600 / size - 300,
											y * 600 / size - 300,
											(z + 0.5) * 600 / size - 300
										);
										var scale = 1 / size;
										cube.scale.set(scale * 0.95, scale * 0.95, scale);
										var material:MeshLambertMaterial = cast cube.material;
										material.color.setHex(0x11DD55);
										material.opacity = 0.1;
										material.transparent = true;
										cube.visible = true;
										cube.receiveShadow = false;
										cube.castShadow = false;
										cube.rotation.set(0, 0, 0);
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
								if (nextY == size || (!game.targetModel[x][nextY][z] && !game.currentModel[x][nextY][z]))
								{
									if (targetY == null)
									{
										var cube = getCube(count);
										cube.position.set(
											x * 600 / size - 300,
											(y + 0.5) * 600 / size - 300,
											z * 600 / size - 300
										);
										var scale = 1 / size;
										cube.scale.set(scale * 0.95, scale * 0.95, scale);
										var material:MeshLambertMaterial = cast cube.material;
										material.color.setHex(0x11DD55);
										material.opacity = 0.1;
										material.transparent = true;
										cube.visible = true;
										cube.receiveShadow = false;
										cube.castShadow = false;
										cube.rotation.set( -Math.PI / 2, 0, 0);
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
								
								currentY = null;
								currentZ = null;
							}
							else
							{
								currentY = null;
								currentZ = null;
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