// vulkan NDC:	x: -1(left), 1(right)
//				y: -1(top), 1(bottom)

attribute vec3 inPosition;
attribute vec2 inTexCoord;

uniform mat4x4 g_ModelViewProjectionMatrix;

varying vec2 fragTexCoord;

void main() {

	gl_Position = vec4(inPosition, 1.0) * g_ModelViewProjectionMatrix;

	fragTexCoord = inTexCoord;
	
}