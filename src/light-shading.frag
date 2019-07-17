#version 330 core

out vec4 out_color;
in vec2 TexCoord;
in vec3 normals;
in vec3 toLightVector;
in vec3 toCameraVector;

uniform vec3 color;
uniform sampler2D image;
uniform vec3 l_ambient;
uniform vec3 l_diffuse;
uniform vec3 l_specular;

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;

uniform float shineDamper;
uniform float reflectivity;
uniform int useTextures;

//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.75;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 1.0;

void main(void) {


	
	//VIGNETTE
	
	//determine center position
	vec2 position = (gl_FragCoord.xy / vec2(1280.0, 720.0)) - vec2(0.5);
	
	//determine the vector length of the center position
	float len = length(position);
	
	//use smoothstep to create a smooth vignette
	float vignette = smoothstep(RADIUS, RADIUS-SOFTNESS, len);
	
	//apply the vignette with 50% opacity
	
	float specularFactor = 0.0;

	vec3 surfaceNormal = vec3(0.0, 0.0, 0.0) + normals;

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);
	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.5);
	vec3 diffuse = brightness * l_diffuse * m_diffuse;

	//need normalised tocam + light
	float distance = length(toLightVector);
	distance = distance * distance;
	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 halfDir = normalize(unitLightVector + unitVectorToCamera);
	float specularAngle = max(dot(halfDir, normalize(surfaceNormal)), 0.0);
	specularFactor = specularAngle;

	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 finalSpecular = dampedFactor * l_specular * m_specular * reflectivity;

	//simply multiply light colours by material colors, pass in light mode, then hardcode colors in
	//get material[0] mabient diffuse and specular

	vec3 ambient = m_ambient * l_ambient;


	vec4 vigColor = texture(image, TexCoord);
	vigColor.rgb = mix(vigColor.rgb, vigColor.rgb * vignette, 1.0);

	if(useTextures == 0) {
		out_color = vec4(diffuse, 1.0)* vigColor + vec4(finalSpecular, 1.0);
	} else {
		out_color = vigColor;
	}
	
	
	//out_color = vec4(0.5, 0.5, 0.5, 1.0);
}
