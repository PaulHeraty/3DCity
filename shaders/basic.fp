#ifdef GL_ES
precision mediump float;
#endif
varying vec2 v_texCoord;
uniform sampler2D s_texture;
uniform int u_wireframe;

void main()
{
  if(u_wireframe == 1) {
    // Draw wireframe, set color to white
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    // Draw normal, get color from texture
    gl_FragColor = texture2D( s_texture, v_texCoord);
  }
}

