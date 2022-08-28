#version 450 core

in vec4 fColor;
in vec2 fTexCoord;
in float fTexID;

uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 color2;

void main() {
	vec3 color = texture(texSampler, fTexCoord).rgb;
	outColor = vec4(color, 1.0);
	
	color2 = vec4(1.0 - color, 1.0f);
}