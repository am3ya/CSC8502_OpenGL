#version 330 core

uniform float fadeFactor; // Opacity: 0.0 (transparent) to 1.0 (fully black)

out vec4 fragColour; // Final output color

void main() {
    fragColour = vec4(0.5, 0.0, 0.0, fadeFactor); // Black with adjustable alpha
}