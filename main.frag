#version 410

layout (location=0) out vec4 outColor;

in vec2 texCoordFrag;
in vec3 tintFrag;

uniform sampler2D tex;

void main() {
    vec4 tex = texture(tex, texCoordFrag);
    outColor = vec4(tintFrag * tex.xyz, tex.w);
}
