#version 330 core
in vec2 FragPos;
out vec4 FragColor;

uniform vec4 uColor;

uniform vec4 uRect;
uniform float uRounding;

float roundrect(vec2 uv, vec2 size, float radius) {
    vec2 d = abs(uv) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

void main() { 
    vec2 rect_center = uRect.xy + uRect.zw * 0.5;
    vec2 uv = FragPos - rect_center;

    float dist = roundrect(uv, uRect.zw * 0.5, uRounding);
    
    float insideMask = step(dist, 0.0);
    FragColor = mix(vec4(0.0), uColor, insideMask);
}
