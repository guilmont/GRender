#version 430 core

in vec4 fColor;
in vec2 fCoord;
in float texID;

uniform sampler2D u_texture[2];

out vec4 fragColor;

void main()
{

    fragColor = fColor * texture(u_texture[int(texID)], fCoord);

    // fragColor = vec4(fCoord.x, fCoord.y, 0.0, 1.0);


}