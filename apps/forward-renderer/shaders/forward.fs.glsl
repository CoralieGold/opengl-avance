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

in vec3 vNormal_vs;
in vec3 vPosition_vs;
in vec2 vTexCoords;

out vec3 fFragColor;

void main() {
	float distToPointLight = length(uPointLightPosition - vPosition_vs);
	vec3 dirToPointLight = (uPointLightPosition - vPosition_vs) / distToPointLight;

	vec3 uKd3 = uKd * vec3(texture(uKdSampler, vTexCoords));
	fFragColor = uKd3 * (uDirectionalLightIntensity * max(0.0, dot(vNormal_vs, uDirectionalLightDirect)) + uPointLightIntensity * max(0.0, dot(vNormal_vs, dirToPointLight)) / (distToPointLight * distToPointLight));

	fFragColor = vNormal_vs;
};