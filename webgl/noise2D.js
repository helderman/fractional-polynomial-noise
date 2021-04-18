const fragmentShaderSource = `

// ------------------------------------
// GLSL code
// ------------------------------------

precision lowp float;

#if __VERSION__ == 300
#define gl_FragColor FragColor
out vec4 gl_FragColor;
#define overlap u_overlap
#else
#define overlap 4
#endif

// -------------------- Parameters -----------------------

uniform vec2 u_center;
uniform float u_scale;
uniform int u_overlap;
uniform float u_smooth;

// -------------------- PRNG -----------------------

// "Hash without Sine" by David Hoskins
// MIT License
// https://www.shadertoy.com/view/4djSRW
vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yxz+33.33);
	return fract((p3.xxy+p3.yzz)*p3.zyx);
}

// -------------------- Noise calculation -----------------------

// Calculates the height for a single layer
float f(in vec2 cell, in vec2 local)
{
	vec2 carrier = 1.0 - local * local;
	return dot(hash32(cell) - 0.5, vec3(local, 1.0)) *
		pow(carrier.x * carrier.y, u_smooth);
}

// Calculates the total height, using all layers
float g(in vec2 position)
{
	float total = 0.0;
	float d = float(overlap);
	for (int j = 0; j < overlap; j++)
	{
		for (int i = 0; i < overlap; i++)
		{
			vec2 ij = vec2(i, j);
			vec2 p = (position + ij) / d;
			total += f(d * floor(p) - ij, 2.0 * fract(p) - 1.0);
		}
	}
	return total;
}

// -------------------- demo -----------------------

void main()
{
	vec2 pos = u_scale * (gl_FragCoord.xy - u_center);
	float h = g(pos) * 0.2 + 0.5;
	gl_FragColor = vec4(vec3(h), 1.0);
}

`;

// ------------------------------------
// JavaScript code
// ------------------------------------

const ver1 = window.location.hash == '#1';
const version = ver1 ? '#version 100' : '#version 300 es';
const contextType = ver1 ? 'webgl' : 'webgl2';
document.getElementById('i_overlap').disabled = ver1;

// In our HTML page, there should be a canvas with ID 'canvas'.
const canvas = document.getElementById('canvas');
const gl = canvas.getContext(contextType);
if (!gl) throw contextType + ' not supported here';

// Create a 'program' with a vertex shader and a fragment shader.
const program = gl.createProgram();
const loadShader = function(gl, type, source) {
	const shader = gl.createShader(type);
	gl.shaderSource(shader, version + source);
	gl.compileShader(shader);
	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		throw 'Compile error: ' + gl.getShaderInfoLog(shader);
	}
	return shader;
};
gl.attachShader(program, loadShader(gl, gl.VERTEX_SHADER, `
#if __VERSION__ == 300
#define attribute in
#define varying out
#endif
	attribute vec2 a_position;	// clip space [-1, +1]
	varying vec2 v_texCoord;	// texel coordinates [0, 1]
	void main() {
		gl_Position = vec4(a_position, 0.0, 1.0);
		v_texCoord = (a_position + 1.0) * 0.5;
	}

`));
gl.attachShader(program, loadShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource));
gl.linkProgram(program);
if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
	throw 'Link error: ' + gl.getProgramInfoLog(program);
}

// 'Attributes' and 'uniforms' are used to pass parameters into the shaders.
// Each one has a unique number called a 'location'. Collecting those here.
const locations = {
	attribute: function(name) {
		this[name] = gl.getAttribLocation(program, name);
	},
	uniform: function(name) {
		this[name] = gl.getUniformLocation(program, name);
	}
};
locations.uniform('u_center');
locations.uniform('u_scale');
locations.uniform('u_overlap');
locations.uniform('u_smooth');
locations.attribute('a_position');

// Prepare the vertex shader with a buffer containing 6 vertices (2 triangles).
const vertexShader = {
	positionBuffer: gl.createBuffer(),
	draw: function(positionLocation) {
		gl.enableVertexAttribArray(positionLocation);
		gl.bindBuffer(gl.ARRAY_BUFFER, this.positionBuffer);
		gl.vertexAttribPointer(positionLocation, 2, gl.FLOAT, false, 0, 0);
		gl.drawArrays(gl.TRIANGLES, 0, 6);
	}
};
gl.bindBuffer(gl.ARRAY_BUFFER, vertexShader.positionBuffer);
gl.bufferData(
	gl.ARRAY_BUFFER,
	new Float32Array([ -1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, 1 ]),
	gl.STATIC_DRAW);

// Canvas size will always follow window size.
(window.onresize = function() {
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;
})();

function getInt(name) {
	let value = parseInt(document.getElementById('i_' + name).value);
	document.getElementById('o_' + name).innerHTML = value;
	return value;
}

function getFloat(name) {
	let value = parseFloat(document.getElementById('i_' + name).value);
	document.getElementById('o_' + name).innerHTML = value.toFixed(1);
	return value;
}

// Animation loop.
(function loop(time) {
	requestAnimationFrame(loop);

	gl.viewport(0, 0, canvas.width, canvas.height);
	gl.useProgram(program);
	gl.uniform2f(locations.u_center, canvas.width / 2, canvas.height / 2);
	gl.uniform1f(locations.u_scale, Math.exp(-getFloat('scale')));
	gl.uniform1i(locations.u_overlap, getInt('overlap'));
	gl.uniform1f(locations.u_smooth, getFloat('smooth'));

	vertexShader.draw(locations.a_position);
})(0);
