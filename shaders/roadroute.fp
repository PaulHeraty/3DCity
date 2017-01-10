#extension GL_OES_standard_derivatives : enable

#ifdef GL_ES
precision mediump float;
#endif
varying vec2 v_texCoord;
varying vec3 v_barycentric;
uniform sampler2D s_texture;
uniform int u_wireframe;
uniform int u_highlight;

float edgeFactor() {
  vec3 d = fwidth(v_barycentric);
  vec3 a3 = smoothstep(vec3(0.0), d*1.5, v_barycentric);
  return min(min(a3.x, a3.y), a3.z);
}

void main()
{
  if(u_wireframe == 1) {
    gl_FragColor.rgb = mix(vec3(0.5), vec3(1.0), edgeFactor());
    gl_FragColor.a = 1.0;
  } else if(u_wireframe == 2) {
    gl_FragColor.rgb = mix(vec3(0.0), vec3(0.5), edgeFactor());
    gl_FragColor.a = 0.1;
  } else {
    vec4 tempColor;
    tempColor = texture2D( s_texture, v_texCoord);

    if(u_highlight == 0) {
      // Add pink hue
      tempColor *= vec4(0.1, 0.1, 0.1, 1.0);
    }

    // Since textures are in BGRA format, need to swap to RGBA
    gl_FragColor.r = tempColor.b;
    gl_FragColor.g = tempColor.g;
    gl_FragColor.b = tempColor.r;
    gl_FragColor.a = tempColor.a;
  }
}

