#version 430

uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAmbient;
uniform sampler2D uGDiffuse;
uniform sampler2D uGGlossyShininess;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

out vec3 fFragColor;

void main() {
	// float distToPointLight = length(uPointLightPosition - vPosition_vs);
	// vec3 dirToPointLight = (uPointLightPosition - vPosition_vs) / distToPointLight;

	// vec3 uKd3 = vec3(texture(uKdSampler, vTexCoords));
	// uKd3 += vec3(texture(uKaSampler, vTexCoords));
	// uKd3 += vec3(texture(uKsSampler, vTexCoords));
	// uKd3 += vec3(texture(uShininessSampler, vTexCoords));
	// uKd3 *= uKd;
	// fFragColor = uKd3 * (uDirectionalLightIntensity * max(0.0, dot(vNormal_vs, uDirectionalLightDirect)) + uPointLightIntensity * max(0.0, dot(vNormal_vs, dirToPointLight)) / (distToPointLight * distToPointLight));

	// fFragColor = vNormal_vs;

        vec3 position = vec3(texelFetch(uGPosition, ivec2(gl_FragCoord.xy), 0));
        vec3 normal = vec3(texelFetch(uGNormal, ivec2(gl_FragCoord.xy), 0));

        vec3 ka = vec3(texelFetch(uGAmbient, ivec2(gl_FragCoord.xy), 0));
        vec3 kd = vec3(texelFetch(uGDiffuse, ivec2(gl_FragCoord.xy), 0));
        vec4 ksShininess = texelFetch(uGGlossyShininess, ivec2(gl_FragCoord.xy), 0);
        vec3 ks = ksShininess.rgb;
        float shininess = ksShininess.a;

        vec3 eyeDir = normalize(-position);

        float distToPointLight = length(uPointLightPosition - position);
        vec3 dirToPointLight = (uPointLightPosition - position) / distToPointLight;
        vec3 pointLightIncidentLight = uPointLightIntensity / (distToPointLight * distToPointLight);

        // half vectors, for blinn-phong shading
        vec3 hPointLight = normalize(eyeDir + dirToPointLight);
        vec3 hDirLight = normalize(eyeDir + uDirectionalLightDir);

        float dothPointLight = shininess == 0 ? 1.f : max(0.f, dot(normal, hPointLight));
        float dothDirLight = shininess == 0 ? 1.f :max(0.f, dot(normal, hDirLight));

        if (shininess != 1.f && shininess != 0.f)
        {
            dothPointLight = pow(dothPointLight, shininess);
            dothDirLight = pow(dothDirLight, shininess);
        }

        fFragColor = ka;
        fFragColor += kd * (uDirectionalLightIntensity * max(0.f, dot(normal, uDirectionalLightDir)) + pointLightIncidentLight * max(0., dot(normal, dirToPointLight)));
        fFragColor += ks * (uDirectionalLightIntensity * dothDirLight + pointLightIncidentLight * dothPointLight);
}
