#version 330 core

// Stocker les paramètres d'une lumière directionnelle
uniform vec3 uDirectionalLightDirect;
uniform vec3 uDirectionalLightIntensity;

// Stocker les paramètres d'une lumière ponctuelle
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

// Stocker la couleur diffuse
uniform vec3 uKd;

// Texture
uniform sampler2D uKdSampler;
uniform sampler2D uKaSampler;
uniform sampler2D uKsSampler;
uniform sampler2D uShininessSampler;
uniform float uShininess;

in vec3 vNormal_vs;
in vec3 vPosition_vs;
in vec2 vTexCoords;

// out vec3 fFragColor;

// Textures en sortie
layout(location = 0) out vec3 fPosition;
layout(location = 1) out vec3 fNormal;
layout(location = 2) out vec3 fAmbient;
layout(location = 3) out vec3 fDiffuse;
layout(location = 4) out vec4 fGlossyShininess;


void main() {
	// float distToPointLight = length(uPointLightPosition - vPosition_vs);
	// vec3 dirToPointLight = (uPointLightPosition - vPosition_vs) / distToPointLight;

	// vec3 uKd3 = vec3(texture(uKdSampler, vTexCoords));
	// uKd3 += vec3(texture(uKaSampler, vTexCoords));
	// uKd3 += vec3(texture(uKsSampler, vTexCoords));
	// uKd3 += vec3(texture(uShininessSampler, vTexCoords));
	// uKd3 *= uKd;
	// fFragColor = uKd3 * (uDirectionalLightIntensity * max(0.0, dot(vNormal_vs, uDirectionalLightDirect)) + uPointLightIntensity * max(0.0, dot(vNormal_vs, dirToPointLight)) / (distToPointLight * distToPointLight));


	fPosition = vPosition_vs;
	fNormal = normalize(vNormal_vs);
	fDiffuse = uKd * vec3(texture(uKdSampler, vTexCoords));
	fAmbient = uKd * vec3(texture(uKaSampler, vTexCoords));
	fGlossyShininess = vec4(uKd * vec3(texture(uKaSampler, vTexCoords)), uShininess);
};