#version 330 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform float ambientStrength;

struct PointLight {
	vec3 position;
	vec3 color;
};
uniform PointLight pointLight;

struct DirectionalLight {
	vec3 direction;
	vec3 color;
};
uniform DirectionalLight dirLight;

uniform vec3 attenCoeff = vec3(1.0, 0.0, 0.0);

uniform float specularStrength;
uniform vec3 viewPos;
uniform sampler2D aTexture;

vec3 CalcPointLight(PointLight light) {
	// calculate attenuation coefficient based on distance from light
	float lightDistance = length(light.position - FragPos);
	float attenuation = 1.0 / (attenCoeff.x + attenCoeff.y * lightDistance + attenCoeff.z * (lightDistance * lightDistance));

	// calculate ambient lighting
	vec3 ambient = light.color * ambientStrength;

	// calculate diffuse lighting
	vec3 norm = normalize(Normal);								// normalize Normal vector incase does not already
																	// have a magnitude of 1
	vec3 lightDir = normalize(light.position - FragPos);		// calculate direction of ray hitting fragment and
																	// normalize the result
	float diff = max(dot(norm, lightDir), 0.0);					// calculate how bright the fragment should be based
																	// on the angle between the normal and ray of light
	vec3 diffuse = diff * light.color;

	// calculate specular lighting
	vec3 viewDir = normalize(viewPos - FragPos);				// calculate view direction and normalize
	vec3 reflectDir = reflect(-lightDir, norm);					// calculate direction of reflected light
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);	// calculate specular constant

	vec3 specular = specularStrength * spec * light.color;
	
	// adjust for attenuation
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirectionalLight light) {
	// calculate light direction
	vec3 lightDir = normalize(-light.direction);

	// calculate ambient lighting
	vec3 ambient = light.color * ambientStrength;

	// calculate diffuse lighting
	vec3 norm = normalize(Normal);								// normalize Normal vector incase does not already
																	// have a magnitude of 1
	float diff = max(dot(norm, lightDir), 0.0);					// calculate how bright the fragment should be based
																	// on the angle between the normal and ray of light
	vec3 diffuse = diff * light.color;

	// calculate specular lighting
	vec3 viewDir = normalize(viewPos - FragPos);				// calculate view direction and normalize
	vec3 reflectDir = reflect(-lightDir, norm);					// calculate direction of reflected light
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);	// calculate specular constant

	vec3 specular = specularStrength * spec * light.color;

	return (ambient + diffuse + specular);
}

void main()
{
	// calculate fragment color
	FragColor = vec4(CalcPointLight(pointLight) + CalcDirLight(dirLight), 1.0) * texture(aTexture, TexCoord);
}