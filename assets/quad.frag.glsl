#version 450 core

in vec4 fColor;
in vec2 fTexCoord;
in flat int fTexID;

uniform sampler2D texSampler[32];

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 color2;

void main() {

	vec3 color = fColor.rgb;
	if (fTexID >= 0) { color *= texture(texSampler[fTexID], fTexCoord).rgb; }

	outColor = vec4(color, 1.0);
	color2 = vec4(1.0 - color, 1.0f);
}