#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 com;
layout(location = 4) in vec3 direction;

uniform mat4 u_transform;
uniform float u_radius;

out vec3 fPos;
out vec3 fNormal;
out vec3 fColor;

void main()
{
    float dist = length(direction);
    vec3 loc = vec3(dist * pos.x, u_radius * pos.y, u_radius * pos.z);

    vec3 rx = normalize(direction);
    vec3 ry = normalize(vec3(-(rx.y + rx.z + 2e-5)/(rx.x + 1e-5), 1.0f, 1.0f));
    vec3 rz = cross(rx, ry);

    vec3 vp, vn;
    vp.x = rx.x * loc.x + ry.x * loc.y + rz.x * loc.z + com.x; 
    vp.y = rx.y * loc.x + ry.y * loc.y + rz.y * loc.z + com.y; 
    vp.z = rx.z * loc.x + ry.z * loc.y + rz.z * loc.z + com.z; 

    vn.x = rx.x * normal.x + ry.x * normal.y + rz.x * normal.z; 
    vn.y = rx.y * normal.x + ry.y * normal.y + rz.y * normal.z; 
    vn.z = rx.z * normal.x + ry.z * normal.y + rz.z * normal.z; 


    gl_Position = u_transform * vec4(vp, 1.0);
    
    fPos = vp;
    fNormal = normalize(vn);
    fColor = color;
}

