#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in int  texID;

uniform mat4 u_transform;

out vec4 fColor;
out vec2 fTexCoord;
out flat int fTexID;

void main() {
    fColor = color;
    fTexCoord = texCoord;
    fTexID = texID;
    
    gl_Position = u_transform * vec4(position, 1.0);

}
