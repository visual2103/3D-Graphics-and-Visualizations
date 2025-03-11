#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fPosEye;
out vec3 fNormalEye ; 
out vec2 fTexCoords;
out vec4 fragPosLightSpace;

uniform	mat3 normalMatrix; // for calculating the transformed normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceTrMatrix;


void main() 
{
	vec4 posEye4 = view * model * vec4(vPosition, 1.0f);
        fPosEye = posEye4.xyz;
	
	fNormalEye = normalize(normalMatrix * vNormal);
        
        fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);

	fTexCoords = vTexCoords;

	gl_Position = projection * posEye4;  
}
