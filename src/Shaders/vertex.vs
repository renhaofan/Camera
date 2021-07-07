#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
        //###################################################################################
        // The resulting vertex should then be assigned to gl_Position in the vertex shader#
        // and OpenGL will then automatically perform perspective division and clipping.####
        //###################################################################################
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
