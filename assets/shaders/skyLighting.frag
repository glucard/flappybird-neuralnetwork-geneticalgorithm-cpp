// skyLighting.frag
uniform sampler2D texture;      // The object's texture
uniform vec2 moonPos;           // Position of the "moon" or light in window coordinates
uniform vec3 skyColor;          // Color of the sky/moon light (e.g., pale gray for moonlight)
uniform float ambientStrength;  // Base ambient lighting
uniform float radius;           // How far the light extends

void main()
{
    // Sample the base texture color of the object
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);

    // Distance from this fragment to the light source
    float dist = length(gl_FragCoord.xy - moonPos);

    // Attenuation: from 1.0 (near the light) down to 0.0 (far)
    // clamp() prevents values going below 0 or above 1
    float attenuation = 1.0 - clamp(dist / radius, 0.0, 1.0);

    // Combine ambient + attenuated sky color
    float brightness = ambientStrength + attenuation;
    vec3 litColor = texColor.rgb * (skyColor * brightness);

    // Final fragment color
    gl_FragColor = vec4(litColor, texColor.a);
}
