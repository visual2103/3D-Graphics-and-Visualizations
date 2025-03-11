#version 410 core


in vec2 fTexCoords;
in vec4 fragPosLightSpace;

in vec3 fPosEye ; 
in vec3 fNormalEye ; 

out vec4 fColor; // the final output (color of pixel)

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// directional light
uniform vec3 lightDir;
uniform vec3 lightColor;


// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform sampler2D waterTexture;

//point light 
uniform vec3 pointLightPosition1;
uniform vec3 pointLightColor1;
uniform vec3 pointLightPosition2;
uniform vec3 pointLightColor2;

//fog
uniform float fogDensity;
uniform float is_light;

//components for lighting 

float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;

//atenuare

float constant = 1.0f;
float linear = 0.09f;
float quadratic = 0.04f;

vec3 ambient;
vec3 diffuse;
vec3 specular;

//lightning 
uniform float lightningIntensity;



void computeDirLight()
{
    vec3 N = normalize(fNormalEye);
    vec3 L = normalize(lightDir); // prespunem e deja în eye space
    vec3 V = normalize(-fPosEye); // direcția către cameră (eye space => camera la (0,0,0))

    // diffuse
    float diff = max(dot(N, L), 0.0);
    diffuse = diff * lightColor;

    // specular
    vec3 R = reflect(-L, N);
    float specCoeff = pow(max(dot(V, R), 0.0), shininess);
    specular = specularStrength * specCoeff * lightColor;
    
    ambient = ambientStrength * lightColor;
}


vec3 computePointLight(vec3 pointLightPositionEye, vec3 pointLightColor)
{
    vec3 N = normalize(fNormalEye);
    vec3 toLight = pointLightPositionEye - fPosEye;  // totul in eye-space
    float distance = length(toLight);
    toLight = normalize(toLight);

    float diff = max(dot(N, toLight), 0.0);

    // specular
    vec3 V = normalize(-fPosEye); 
    vec3 R = reflect(-toLight, N);
    float spec = pow(max(dot(V, R), 0.0), shininess);

    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    // extragem culorile / texturile
    vec3 texDiff   = vec3(texture(diffuseTexture, fTexCoords));
    vec3 texSpec   = vec3(texture(specularTexture, fTexCoords));

    vec3 ambient2  = ambientStrength * texDiff;
    vec3 diffuse2  = diff * texDiff ;
    vec3 specular2 = spec * texSpec ;

    return attenuation * (ambient2 + diffuse2 + specular2) * pointLightColor;
}


float computeFog()
{
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2.0));
    return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    if (projCoords.z > 1.0f)
        return 0.0f;

    projCoords = projCoords * 0.5f + 0.5f; //normalize 
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    //bias -> lightDir is eye-space but shadowMap is world-space

    vec3 N = normalize(fNormalEye);
    float bias = 0.005f;

    return (currentDepth - bias > closestDepth) ? 1.0f : 0.0f;
}

void main()
{
    // Extragem culoarea din textura, inclusiv canalul alfa
    vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);

    // Eliminăm fragmentul dacă valoarea canalului alfa este mai mică de 0.1
    if(colorFromTexture.a < 0.1)
        discard;

    // Calculăm iluminarea direcțională
    computeDirLight();
    float shadow = computeShadow(fragPosLightSpace);

    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);
     
    vec3 diffTex = colorFromTexture.rgb; // Folosim culoarea din textura difuză
    vec3 specTex = vec3(texture(specularTexture, fTexCoords));

    ambient *= diffTex; 
    diffuse *= diffTex;
    specular *= specTex;

    vec3 color = ambient + diffuse + specular;
   
    // Adăugăm contribuția luminii de tip point light, dacă este activată
    if (is_light > 0.5) {
        color += computePointLight(pointLightPosition1, pointLightColor1);
        color += computePointLight(pointLightPosition2, pointLightColor2);
    }

    // Efectul de fulger
    vec3 lightningEffect = color * lightningIntensity;

    // Calculăm ceața (fog)
    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    // Amestecăm culoarea finală cu ceața
    fColor = mix(fogColor, vec4(lightningEffect, colorFromTexture.a), fogFactor);
}
