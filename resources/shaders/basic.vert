#version 450

// vulkan NDC:	x: -1(left), 1(right)
//				y: -1(top), 1(bottom)

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(push_constant) uniform globals {
	mat4x4 modelViewProjection;
};

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {

	gl_Position = vec4(inPosition, 1.0) * modelViewProjection;

	fragColor = inColor;
	fragTexCoord = inTexCoord;
	
}