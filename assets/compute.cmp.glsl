#version 430

layout (local_size_x = 32, local_size_y = 32) in;

layout (rgba32f, binding = 0) uniform image2D img_output;

uniform float time;

void main() {
	ivec2 coords = ivec2(gl_GlobalInvocationID);

	if ( ((gl_WorkGroupID.x & 1u) != 1u) != ((gl_WorkGroupID.y & 1u) == 1u)) {
		imageStore(img_output, coords, vec4(0.0, abs(cos(time)), 0.0, 1.0));
	} else {
		imageStore(img_output, coords, vec4(1.0, 0.0, 0.0, 1.0));
	}
}