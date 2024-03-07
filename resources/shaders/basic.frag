varying vec2 fragTexCoord;

uniform sampler2D texSampler;

void main() {
	gl_FragColor = texture(texSampler, fragTexCoord);
}