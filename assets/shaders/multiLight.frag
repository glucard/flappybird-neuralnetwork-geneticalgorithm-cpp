uniform sampler2D texture;
uniform int numLights;
uniform vec2  lightPos[8];
uniform float lightRadius[8];
uniform float lightShapeRadius[8];
uniform vec3  lightColor[8];
uniform float ambientStrength;
uniform vec2 screen_resolution;
uniform sampler2D occlusionTexture;
uniform float specularStrength;
uniform float shininess;
uniform float flareIntensity; // Control overall flare strength (0 to 1 recommended)
uniform float attenuation_factor[8];


// Screen blending function to preserve underlying colors
vec3 screenBlend(vec3 base, vec3 blend) {
    return 1.0 - (1.0 - base) * (1.0 - blend);
}

void main()
{
    vec2 sfmlCoord = vec2(gl_FragCoord.x, screen_resolution.y - gl_FragCoord.y);
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Base color with ambient lighting
    vec3 finalColor = texColor.rgb * ambientStrength;
    vec3 totalFlare = vec3(0.0);
    
    vec3 N = vec3(0.0, 0.0, 1.0);
    vec3 V = vec3(0.0, 0.0, 1.0);
    
    for (int i = 0; i < numLights; i++)
    {
        // Existing visibility calculation
        int radialSteps = 4;
        int angularSteps = 8;
        float totalSamples = float(radialSteps * angularSteps);
        float sumVisible = 0.0;
        
        for (int r = 0; r < radialSteps; r++) {
            float normalizedRadius = float(r + 1) / float(radialSteps);
            for (int a = 0; a < angularSteps; a++) {
                float angle = 2.0 * 3.14159265 * float(a) / float(angularSteps);
                vec2 offset = vec2(cos(angle), sin(angle)) * (normalizedRadius * lightShapeRadius[i]);
                vec2 samplePos = lightPos[i] + offset;
                vec2 sampleCoord = samplePos / screen_resolution;
                sumVisible += (1.0 - texture2D(occlusionTexture, sampleCoord).a);
            }
        }
        float visibleFraction = clamp(sumVisible / totalSamples, 0.0, 1.0);
        
        // Distance calculations
        float dist = length(sfmlCoord - lightPos[i]);
        float attenuation = 1.0 - clamp(dist / lightRadius[i], 0.0, 1.0);
        float lightIntensity = attenuation_factor[i] * attenuation * visibleFraction;

        // Diffuse and specular (existing implementation)
        vec3 diffuse = texColor.rgb * lightColor[i] * lightIntensity;
        vec2 L2D = normalize(lightPos[i] - sfmlCoord);
        vec3 L = vec3(L2D, 0.0);
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        vec3 specular = lightColor[i] * specularStrength * spec * visibleFraction;
        
        finalColor += diffuse + specular;

        // Flare effects (new implementation with color preservation)
        vec2 toLightVec = lightPos[i] - sfmlCoord;
        float distToLight = length(toLightVec);
        
        // Flare attenuation curve
        float flareAttenuation = pow(1.0 - clamp(distToLight / (lightRadius[i] * 2.0), 0.0, 1.0), 2.0);
        
        // 1. Subtle core glow
        float glow = 1.0 - smoothstep(0.0, lightShapeRadius[i] * 3.0, distToLight);
        vec3 glowColor = lightColor[i] * pow(glow, 4.0) * 0.3 * flareIntensity;
        
        // 2. Directional streaks
        vec2 dirToLight = normalize(toLightVec);
        float streak = pow(abs(dot(dirToLight, vec2(0.707, 0.707))), 8.0) * 
                     (1.0 - smoothstep(0.0, lightRadius[i] * 1.5, distToLight));
        vec3 streakColor = lightColor[i] * streak * 0.2 * flareIntensity;
        
        // 3. Subtle halo
        float halo = (sin(distToLight * 0.5) * 0.5 + 0.5) * 
                   pow(1.0 - smoothstep(0.0, lightRadius[i], distToLight), 2.0);
        vec3 haloColor = lightColor[i] * halo * 0.15 * flareIntensity;
        
        // Combine flare elements with visibility and attenuation
        vec3 lightFlare = (glowColor + streakColor + haloColor) * 
                         flareAttenuation * visibleFraction * attenuation_factor[i];
        
        // Add to flare accumulator
        totalFlare += lightFlare;
    }
    
    // Apply screen blending to preserve texture colors
    finalColor = screenBlend(finalColor, totalFlare);
    
    // Maintain original texture alpha
    gl_FragColor = vec4(finalColor, texColor.a);
}