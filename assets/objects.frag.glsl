#version 450 core

in vec3 fPos;
in vec3 fNormal;
in vec4 fColor;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 lightPos = vec3(0.0,100.0,50.0);
    vec3 lightDir = normalize(lightPos - fPos);

    float ambientLight = 0.3;
    float diffuse = max(dot(fNormal, lightDir), 0.0);

    vec3 color = fColor.rgb * (ambientLight + diffuse);
    fragColor = vec4(color, 1.0);
}