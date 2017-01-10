#ifdef GL_ES
precision mediump float;
#endif
varying vec2 v_texCoord;
uniform sampler2D s_texture;
void main()
{
  vec4 tempColor;
  tempColor = texture2D( s_texture, v_texCoord);
  //gl_FragColor = texture2D( s_texture, v_texCoord);
  
  // Since textures are in BGRA format, need to swap to RGBA
  gl_FragColor.r = tempColor.b; 
  gl_FragColor.g = tempColor.g;
  gl_FragColor.b = tempColor.r;
  gl_FragColor.a = tempColor.a;
}

