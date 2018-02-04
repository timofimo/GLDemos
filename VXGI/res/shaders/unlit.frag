#version 450 core

#extension GL_ARB_bindless_texture : require

in vec2 oTexcoord;
in vec3 oNormal;

out vec4 FragColor;

uniform Material
{
	sampler2D albedoTexture, normalTexture, specularTexture, opacityTexture;
	vec3 ambient;
	uint textureMask;
	vec4 diffuse;
	vec3 specular;
	float specularExponent;
	vec4 emissive;
} material;

void main()
{
	vec4 albedo = texture(material.albedoTexture, oTexcoord);
	float opacity = texture(material.opacityTexture, oTexcoord).r;
	if(((material.textureMask & 1) != 0 && albedo.a < 0.9) || ((material.textureMask & 8) != 0 && opacity < 0.9))
		discard;
		
	FragColor = vec4(material.ambient, 0.0) + mix(material.diffuse, albedo, (material.textureMask & 1));	
}