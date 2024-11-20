#version 330 core

layout(location = 0) in vec3 position; // Full-screen quad position
layout(location = 1) in vec2 texCoord; // Texture coordinates (not used here but kept for compatibility)

out vec2 fragTexCoord; // Pass to the fragment shader

void main() {
    fragTexCoord = texCoord;
    gl_Position = vec4(position, 1.0); // Directly set clip-space coordinates
}