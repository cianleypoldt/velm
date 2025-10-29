// Default vertex shader: transforms 3D positions and outputs a constant red color.
// Intended for use with bgfx's shaderc (.sc) pipeline. Name this file `vs_red.sc`
// and pair with a simple fragment shader that reads COLOR0 to output final color.
//
// Conventions:
// - Expects a_position : POSITION attribute.
// - Writes v_color0 : COLOR0 varying (set to solid red).
// - Uses uniform mat4 u_modelViewProj to transform positions.
//
// This shader will render triangles in 3D as solid red.
uniform mat4 u_modelViewProj;

attribute vec3 a_position : POSITION;

varying vec4 v_color0 : COLOR0;

void main()
{
    // Transform vertex position by the model-view-projection matrix.
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

    // Output a constant red color (RGBA).
    v_color0 = vec4(1.0, 0.0, 0.0, 1.0);
}
