#version 330 core
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
in float fragScalar;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D colorMap;
uniform bool useColorMap;
uniform float minScalar;
uniform float maxScalar;

out vec4 FragColor;

void main() {
    // Normalize normal vector
    vec3 norm = normalize(fragNormal);
    
    // Calculate light direction
    vec3 lightDir = normalize(lightPos - fragPos);
    
    // Ambient lighting
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 color;
    if (useColorMap) {
        // Map scalar to texture coordinate
        float normalizedScalar = (fragScalar - minScalar) / (maxScalar - minScalar);
        color = texture(colorMap, vec2(normalizedScalar, 0.5)).rgb;
    } else {
        color = objectColor;
    }
    
    // Combine all lighting components
    vec3 result = (ambient + diffuse + specular) * color;
    FragColor = vec4(result, 1.0);
}
