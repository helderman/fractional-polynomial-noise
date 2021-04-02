const fragmentShaderSource = `#version 100

// ------------------------------------
// GLSL code
// ------------------------------------

precision lowp float;

// -------------------- Parameters -----------------------

const float continuity = 4.67970975809363;
const ivec2 overlap = ivec2(2);
const vec3 salt = vec3(3.0, 2.0, 0.0);

// -------------------- PRNG -----------------------

// Generates 3 pseudo-random numbers in parallel;
// enough for 1 polynomial (i.e. 1 grid cell on 1 layer)
vec3 rand(mat3 a)
{
    // https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
    return fract(sin(a * vec3(12.9898, 78.233, 111.1)) * 43758.5453) * 2.0 - 1.0;
}

// -------------------- Noise calculation -----------------------

// Function (x,y) -> (a1*x + a2*y + b) * (1 - x^2)^n * (1 - y^2)^n
float polynomial(vec3 coeff, vec2 pos)
{
    vec2 p = 1.0 - pos * pos;
    return dot(coeff, vec3(pos, 1.0)) * pow(p.x * p.y, continuity);
}

// Calculates the height for a single layer
float layer(vec2 pos, vec2 alignment)
{
    vec2 aligned = floor(pos * vec2(overlap) - alignment) / vec2(overlap);
    return polynomial(rand(mat3(vec3(aligned.x), vec3(aligned.y), salt)), pos - aligned);
}

// Calculates the total height, using all layers
float height(vec2 pos)
{
    float h = 0.0;
    for (int j = -overlap.y; j < overlap.y; j++)
    {
        for (int i = -overlap.x; i < overlap.x; i++)
        {
            h += layer(pos, vec2(i, j));
        }
    }
    return h / sqrt(float(overlap.x * overlap.y));
}

// -------------------- demo -----------------------

void main()
{
    vec2 pos = gl_FragCoord.xy / 60.0;
    float h = height(pos) * 0.2 + 0.5;
    gl_FragColor = vec4(vec3(h), 1.0);
}

`;

// ------------------------------------
// JavaScript code
// ------------------------------------


// In our HTML page, there should be a canvas with ID 'canvas'.
const canvas = document.getElementById('canvas');
const gl = canvas.getContext('webgl');
if (!gl) throw 'WebGL not supported here';

// Create a 'program' with a vertex shader and a fragment shader.
const program = gl.createProgram();
const loadShader = function(gl, type, source) {
	const shader = gl.createShader(type);
	gl.shaderSource(shader, source);
	gl.compileShader(shader);
	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		throw 'Compile error: ' + gl.getShaderInfoLog(shader);
	}
	return shader;
};
gl.attachShader(program, loadShader(gl, gl.VERTEX_SHADER, `

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

// Animation loop.
(function loop(time) {
	requestAnimationFrame(loop);

	gl.viewport(0, 0, canvas.width, canvas.height);
	gl.useProgram(program);

	vertexShader.draw(locations.a_position);
})(0);
