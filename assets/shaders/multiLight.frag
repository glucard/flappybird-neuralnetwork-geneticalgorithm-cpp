/*
Shader de Iluminação Dinâmica com Efeitos de Lens Flare

Técnicas Principais: 
- Iluminação ambiente, difusa e especular (Modelo Blinn-Phong)
- Amostragem de visibilidade multi-ray (soft shadows)
- Efeitos de lens flare (glow, streaks, halo)
- Composição com screen blending (Porter-Duff)

Introdução:
Este shader implementa um sistema avançado de iluminação dinâmica 2D com:
1. Cálculo de iluminação por múltiplas fontes de luz
2. Oclusão baseada em textura (deferred rendering)
3. Efeitos especiais de lens flare com preservação de cores
4. Modelo de reflexão Blinn-Phong para materiais
*/

// Uniformes ==============================================
uniform sampler2D texture;          // Textura principal (deferred)
uniform int numLights;              // Número de luzes ativas
uniform vec2  lightPos[8];          // Posições das luzes (coord. tela)
uniform float lightRadius[8];       // Raio de influência das luzes
uniform float lightShapeRadius[8];  // Raio da forma da luz (para sampling)
uniform vec3  lightColor[8];        // Cores das luzes (RGB)
uniform float ambientStrength;      // Força da iluminação ambiente
uniform vec2 screen_resolution;     // Resolução da tela
uniform sampler2D occlusionTexture; // Textura de oclusão (deferred)
uniform float specularStrength;     // Intensidade especular (Brillo)
uniform float shininess;            // Expoente especular (Nível de brilho)
uniform float flareIntensity;       // Controle global de flare (0-1)
uniform float attenuation_factor[8];// Fator de atenuação por luz

// Funções ================================================
// Screen Blending (Porter-Duff) - Preserva cores subjacentes
vec3 screenBlend(vec3 base, vec3 blend) {
    return 1.0 - (1.0 - base) * (1.0 - blend);
}

void main()
{
    // Coordenadas corrigidas para SFML
    vec2 sfmlCoord = vec2(gl_FragCoord.x, screen_resolution.y - gl_FragCoord.y);
    
    // Cor base da textura com iluminação ambiente
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    vec3 finalColor = texColor.rgb * ambientStrength;
    vec3 totalFlare = vec3(0.0);
    
    // Vetores para cálculo especular (Modelo Blinn-Phong)
    vec3 N = vec3(0.0, 0.0, 1.0); // Normal da superfície (2D)
    vec3 V = vec3(0.0, 0.0, 1.0); // Vetor de vista (2D)

    // Loop de processamento para cada luz ================
    for (int i = 0; i < numLights; i++)
    {
        // [Técnica: Multi-ray Visibility Sampling]
        // Amostragem radial e angular para determinar visibilidade da luz
        int radialSteps = 4;    // Número de anéis de amostragem
        int angularSteps = 8;   // Amostras por anel
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
        
        // [Técnica: Atenuação Física Aproximada]
        // Cálculo de distância e atenuação da luz
        float dist = length(sfmlCoord - lightPos[i]);
        float attenuation = 1.0 - clamp(dist / lightRadius[i], 0.0, 1.0);
        float lightIntensity = attenuation_factor[i] * attenuation * visibleFraction;

        // [Modelo Blinn-Phong - Jim Blinn]
        // Componente difusa
        vec3 diffuse = texColor.rgb * lightColor[i] * lightIntensity;
        
        // Componente especular
        vec2 L2D = normalize(lightPos[i] - sfmlCoord);
        vec3 L = vec3(L2D, 0.0);
        vec3 H = normalize(L + V); // Vetor half-way
        float spec = pow(max(dot(N, H), 0.0), shininess);
        vec3 specular = lightColor[i] * specularStrength * spec * visibleFraction;
        
        finalColor += diffuse + specular;

        // [Efeito: Lens Flare Cinemático]
        // Cálculo de elementos de flare
        vec2 toLightVec = lightPos[i] - sfmlCoord;
        float distToLight = length(toLightVec);
        
        // 1. Glow Central (Bloom Adaptativo)
        float glow = 1.0 - smoothstep(0.0, lightShapeRadius[i] * 3.0, distToLight);
        vec3 glowColor = lightColor[i] * pow(glow, 4.0) * 0.3 * flareIntensity;
        
        // 2. Streaks Direcionais (Flares Anamórficos)
        vec2 dirToLight = normalize(toLightVec);
        float streak = pow(abs(dot(dirToLight, vec2(0.707, 0.707))), 8.0) * 
                     (1.0 - smoothstep(0.0, lightRadius[i] * 1.5, distToLight));
        vec3 streakColor = lightColor[i] * streak * 2.f * flareIntensity;
        
        // 3. Halo Sutil (Efeito de Difração)
        float halo = (sin(distToLight * 0.5) * 0.5 + 0.5) * 
                   pow(1.0 - smoothstep(0.0, lightRadius[i], distToLight), 2.0);
        vec3 haloColor = lightColor[i] * halo * 0.15 * flareIntensity;
        
        // Combinação com atenuação e visibilidade
        float flareAttenuation = pow(1.0 - clamp(distToLight / (lightRadius[i] * 2.0), 0.0, 1.0), 2.0);
        vec3 lightFlare = (glowColor + streakColor + haloColor) * 
                         flareAttenuation * visibleFraction * attenuation_factor[i];
        

        vec2 sampleCoord = lightPos[i] / screen_resolution;
        if (texture2D(occlusionTexture, sampleCoord).a < 0.1)
            totalFlare += lightFlare;
    }
    
    // [Técnica: Screen Blending (Porter-Duff)]
    // Composição final preservando cores subjacentes
    finalColor = screenBlend(finalColor, totalFlare);
    
    // Mantém alpha original da textura
    gl_FragColor = vec4(finalColor, texColor.a);
}