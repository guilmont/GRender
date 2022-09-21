#version 450 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec3 bPosition;
layout(location = 4) in vec3 bRotate;
layout(location = 5) in vec3 bScale;
layout(location = 6) in vec4 bColor;
layout(location = 7) in int bTexID;

uniform mat4 u_transform;

out flat int  fTexID;
out vec4 fColor;
out vec3 fNormal;
out vec3 fPos;


mat2 Rotate(float angle) {
    float c = cos(angle), s = sin(angle);
    return mat2(c, s, -s, c);
}

void main() {
    mat2 rot;

    // Setup color and texture
    fTexID = bTexID;
    fColor = bColor;

    // Working on position and normal
    fNormal = vNormal;
    fPos = bScale * vPosition;
    
    // Rotate in x
    rot = Rotate(bRotate.x);
    fNormal.zy = rot * fNormal.zy;
    fPos.zy = rot * fPos.zy;

    // Rotate in y
    rot = Rotate(bRotate.y);
    fNormal.xz = rot * fNormal.xz;
    fPos.xz = rot * fPos.xz;

    // Rotate in z
    rot = Rotate(bRotate.z);
    fNormal.xy = rot * fNormal.xy;
    fPos.xy = rot * fPos.xy;

    // Translating position
    fPos += bPosition;

    gl_Position = u_transform * vec4(fPos, 1.0);
}

