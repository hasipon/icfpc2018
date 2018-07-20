package pixi.core;

import haxe.extern.EitherType;

@:native("PIXI.Shader")
extern class Shader {

	/**
	 * Wrapper class, webGL Shader for Pixi.
	 * Adds precision string if vertexSrc or fragmentSrc have no mention of it.
	 *
	 * @class
	 * @extends GLShader
	 * @memberof PIXI
	 * @param gl {WebGLRenderingContext} The current WebGL rendering context
	 * @param vertexSrc {String|Array<String>} The vertex shader source as an array of strings.
	 * @param fragmentSrc {String|Array<String>} The fragment shader source as an array of strings.
	 * @param {object} [attributeLocations] - A key value pair showing which location eact attribute should sit. e.g. {position:0, uvs:1}.
	 * @param {String} [precision] - The float precision of the shader. Options are 'lowp', 'mediump' or 'highp'.
	 */
	function new(gl:Dynamic, vertexSrc:EitherType<String, Array<String>>, framentSrc:EitherType<String, Array<String>>, ?attributeLocations:Dynamic, ?precision:String);

	/**
	 *
	 * @param	src {String|Array<String>}
	 * @return {String|Array<String>}
	 */
	function checkPrecision(src:EitherType<String, Array<String>>):EitherType<String, Array<String>>;

	/**
	 * Uses this shader
	 */
	function bind():Void;

	/**
	 * Destroys this shader
	 */
	function destroy():Void;

	var gl:Dynamic;
	var program:Dynamic;
	var attributes:Dynamic;
	var uniforms:Dynamic;
}