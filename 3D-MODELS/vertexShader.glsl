#version 430
layout (location=0) in vec3 position;
layout (location=2) in vec2 texCoord;
layout (binding=0) uniform sampler2D samp;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
out vec4 varyingColor;
out vec2 tC;
void main(void)
{ 
   gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
   varyingColor = vec4(position,1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
   tC = texCoord;
}
