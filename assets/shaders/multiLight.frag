// multiLight.frag

uniform sampler2D texture;   // The object's texture
uniform int numLights;       // Number of lights in use

// Arrays of light parameters
uniform vec2  lightPos[8];
uniform float lightRadius[8];
uniform vec3  lightColor[8];

uniform float ambientStrength; // Base ambient lighting (0 to 1)

void main()
{
    // Sample the object’s base color
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);

    // Start with a small ambient color
    vec3 finalColor = texColor.rgb * ambientStrength;

    // Accumulate light from each source
    for(int i = 0; i < numLights; i++)
    {
        // Distance from this fragment to light i
        float dist = length(gl_FragCoord.xy - lightPos[i]);
        
        // Simple radial falloff from 1.0 at dist=0 to 0.0 at dist=radius
        float attenuation = 1.0 - clamp(dist / lightRadius[i], 0.0, 1.0);
        
        // Add this light’s contribution to finalColor
        finalColor += texColor.rgb * lightColor[i] * attenuation;
    }

    // Output the final illuminated color
    gl_FragColor = vec4(finalColor, texColor.a);
}
