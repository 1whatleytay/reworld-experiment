#version 410

layout (location=0) in vec2 position;
layout (location=1) in vec2 texCoord;
layout (location=2) in vec3 tint;
layout (location=3) in vec2 origin;
layout (location=4) in float rotation;

out vec2 texCoordFrag;
out vec3 tintFrag;

uniform vec2 screenSize;
uniform vec2 offset;

void main() {
    texCoordFrag = texCoord;
    tintFrag = tint;

    vec2 transform = position - origin;
    vec2 world = vec2(
        transform.x * cos(rotation) - transform.y * sin(rotation),
        transform.y * cos(rotation) + transform.x * sin(rotation)) + origin;

    vec2 screen = world + offset;
    gl_Position = vec4(screen.x / screenSize.x * 2 - 1, -(screen.y / screenSize.y * 2 - 1), 0, 1);
}
