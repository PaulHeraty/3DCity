#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif
varying vec2 v_texCoord;
uniform sampler2D s_texture;

// The inverse of the texture dimensions along X and Y
//uniform vec2 u_texCoordOffset;
uniform float u_texCoordOffsetX;
uniform float u_texCoordOffsetY;

void main()
{
  // Draw normal, get color from texture
  //gl_FragColor = texture2D( s_texture, v_texCoord);

  // Hardcode for now
  //vec2 u_texCoordOffset = vec2(1.0/1200.0, 1.0/800.0);
  vec2 u_texCoordOffset = vec2(u_texCoordOffsetX, u_texCoordOffsetY);

  // the parameters are hardcoded for now, but could be 
  // made into uniforms to control from the app
  float FXAA_SPAN_MAX = 8.0;
  float FXAA_REDUCE_MUL = 1.0/8.0;
  float FXAA_REDUCE_MIN = (1.0/128.0);

  vec3 rgbNW = texture2D(s_texture, v_texCoord.xy + (vec2(-1.0, -1.0) * u_texCoordOffset)).xyz;
  vec3 rgbNE = texture2D(s_texture, v_texCoord.xy + (vec2(+1.0, -1.0) * u_texCoordOffset)).xyz;
  vec3 rgbSW = texture2D(s_texture, v_texCoord.xy + (vec2(-1.0, +1.0) * u_texCoordOffset)).xyz;
  vec3 rgbSE = texture2D(s_texture, v_texCoord.xy + (vec2(+1.0, +1.0) * u_texCoordOffset)).xyz;
  vec3 rgbM  = texture2D(s_texture, v_texCoord.xy).xyz;

  vec3 luma = vec3(0.299, 0.587, 0.114);
  float lumaNW = dot(rgbNW, luma);
  float lumaNE = dot(rgbNE, luma);
  float lumaSW = dot(rgbSW, luma);
  float lumaSE = dot(rgbSE, luma);
  float lumaM  = dot(rgbM,  luma);

  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

  vec2 dir;
  dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

  float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

  float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

  dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), 
            max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * u_texCoordOffset;

  vec3 rgbA = (1.0/2.0) * (
		texture2D(s_texture, v_texCoord.xy + dir * (1.0/3.0 - 0.5)).xyz +
		texture2D(s_texture, v_texCoord.xy + dir * (2.0/3.0 - 0.5)).xyz);
  vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
		texture2D(s_texture, v_texCoord.xy + dir * (0.0/3.0 - 0.5)).xyz +
		texture2D(s_texture, v_texCoord.xy + dir * (3.0/3.0 - 0.5)).xyz);
  float lumaB = dot(rgbB, luma);

  if((lumaB < lumaMin) || (lumaB > lumaMax)) {
    gl_FragColor.xyz = rgbA;
  } else {
    gl_FragColor.xyz = rgbB;
  }
  gl_FragColor.a = 1.0;
}

