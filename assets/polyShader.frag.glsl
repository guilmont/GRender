#version 450 core

in vec3 fPos;
in vec3 fColor;
in vec3 fNormal;


layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 color2;

void main()
{
    vec3 lightPos = vec3(0.0,100.0,0.0);
    vec3 lightDir = normalize(lightPos - fPos);

    float ambientLight = 0.3;
    float diffuse = max(dot(fNormal, lightDir), 0.0);

    vec3 color = fColor * (ambientLight + diffuse);
    fragColor = vec4(color, 1.0);
    color2 = vec4(1.0 - color, 1.0);
}