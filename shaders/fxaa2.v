#version 310 es
//attribute vec4 vVertex;
in vec4 a_position;
//attribute vec2 vTexCoord;
in vec2 a_texCoord;
//varying vec2 v_texCoord;
out vec2 v_texCoord;

void main()
{
   gl_Position = a_position;
   v_texCoord = a_texCoord;
}


