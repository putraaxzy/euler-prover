#version 330 core
in vec3 fragPos;
in float phase;

uniform vec3 viewPos;
uniform sampler2D colorRamp;
uniform float minMagnitude;
uniform float maxMagnitude;
uniform float magnitude; // For fixed color values

out vec4 FragColor;

// Constants
const float PI = 3.14159265359;

// HSV to RGB conversion
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    // Domain coloring based on phase and magnitude
    float normalizedPhase = phase / (2.0 * PI);
    float normalizedMagnitude = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude);
    normalizedMagnitude = clamp(normalizedMagnitude, 0.0, 1.0);
    
    // Use HSV for coloring - hue based on phase, saturation/value based on magnitude
    vec3 hsv = vec3(normalizedPhase, 1.0, 0.8 + 0.2 * normalizedMagnitude);
    vec3 rgb = hsv2rgb(hsv);
    
    // Add contour lines based on phase
    float phaseMod = mod(phase, PI/4.0) / (PI/4.0);
    if (phaseMod < 0.05 || phaseMod > 0.95) {
        rgb *= 0.7; // Darken at phase contours
    }
    
    // Add contour lines based on magnitude
    float magMod = mod(normalizedMagnitude * 5.0, 1.0);
    if (magMod < 0.05 || magMod > 0.95) {
        rgb *= 0.8; // Darken at magnitude contours
    }
    
    FragColor = vec4(rgb, 1.0);
}
