#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 fragPos;
out float phase;

// Constants for domain coloring
const float PI = 3.14159265359;

void main() {
    // Calculate position with time animation for some effects
    vec3 pos = position;
    
    // For domain coloring of complex functions
    phase = atan(pos.y, pos.x);
    if (phase < 0.0) phase += 2.0 * PI;
    
    fragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
