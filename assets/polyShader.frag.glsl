#version 410 core

in vec3 fPos;
in vec3 fColor;
in vec3 fNormal;

out vec4 fragColor;

void main()
{
    vec3 lightPos = vec3(0.0,100.0,0.0);
    vec3 lightDir = normalize(lightPos - fPos);

    float ambientLight = 0.3;
    float diffuse = max(dot(fNormal, lightDir), 0.0);

    fragColor = vec4(fColor * (ambientLight + diffuse), 1.0);


    // fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}