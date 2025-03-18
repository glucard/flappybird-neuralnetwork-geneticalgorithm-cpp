// assets/shaders/lighting.frag
uniform sampler2D texture;      // The texture of the shape.
uniform vec2 lightPos;          // Position of the light source.
uniform vec3 lightColor;        // Color of the light.
uniform float ambientStrength;  // Ambient light factor.

void main()
{
    // Get the original texture color.
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Calculate distance from the fragment to the light source.
    float distance = length(gl_FragCoord.xy - lightPos);
    // Simple attenuation: the farther, the less intense.
    float attenuation = 1.0 / (distance * 0.05);
    
    // Combine ambient and diffuse lighting.
    vec3 lighting = ambientStrength * texColor.rgb + attenuation * lightColor * texColor.rgb;
    gl_FragColor = vec4(lighting, texColor.a);
}
