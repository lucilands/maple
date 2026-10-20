#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 FragPos;

uniform mat4 uViewProjection;
uniform mat4 uTransform;

void main() {
    vec4 world = uTransform * vec4(aPos, 0.0, 1.0);
    FragPos = world.xy;
    gl_Position = uViewProjection * world;
}
