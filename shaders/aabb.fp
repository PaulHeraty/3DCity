#ifdef GL_ES
precision mediump float;
#endif
varying vec2 v_texCoord;
uniform sampler2D s_texture;
uniform int u_wireframe;
uniform vec4 u_color;
void main()
{
  // Draw wireframe, set color to red
  //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  gl_FragColor = u_color;
}

