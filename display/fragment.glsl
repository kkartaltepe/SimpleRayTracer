#version 330 core
in vec2 UV;
out vec3 color;

// Values that stay constant for the whole mesh (are assigned in C++)
uniform sampler2D textureSampler;

void main() {
 color = texture(textureSampler, UV).rbg;
}
