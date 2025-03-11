#version 330 core
in vec2 fragTexCoords;

uniform sampler2D textureSampler; // Textura cu transparență
uniform float alphaThreshold;     // Pragul de transparență (0.5, de exemplu)

out vec4 fragColor;

void main() {
    vec4 texColor = texture(textureSampler, fragTexCoords);

    // Eliminăm fragmentele unde alpha < alphaThreshold
    if (texColor.a < alphaThreshold) {
        discard;
    }

    fragColor = texColor;
}
