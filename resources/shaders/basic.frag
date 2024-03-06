layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 diffuseColor;

void main() {
	diffuseColor = texture(texSampler, fragTexCoord);
}