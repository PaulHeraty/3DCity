#extension GL_OES_standard_derivatives : enable

#ifdef GL_ES
precision mediump float;
#endif
varying vec2 v_texCoord;
varying vec3 v_barycentric;
uniform sampler2D s_texture;
uniform int u_wireframe;

float edgeFactor() {
  vec3 d = fwidth(v_barycentric);
  vec3 a3 = smoothstep(vec3(0.0), d*1.5, v_barycentric);
  return min(min(a3.x, a3.y), a3.z);
}

void main() {
  if(u_wireframe == 1) {
    // Solid mode
    gl_FragColor.rgb = mix(vec3(0.5), vec3(1.0), edgeFactor());
    gl_FragColor.a = 1.0;
  } else if(u_wireframe == 2) {
    // Transparent wireframe mode
    gl_FragColor.rgb = mix(vec3(0.5), vec3(1.0), edgeFactor());
    gl_FragColor.a = 0.1;
  } else {
    // Draw normal, get color from texture
    gl_FragColor = texture2D( s_texture, v_texCoord);
  }
}

