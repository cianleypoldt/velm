// Simple fragment shader that outputs the interpolated color from the vertex shader.
// Expects `v_color0` varying (COLOR0) produced by the vertex shader.
varying vec4 v_color0 : COLOR0;

void main()
{
    // Output the interpolated color directly.
    gl_FragColor = v_color0;
}
