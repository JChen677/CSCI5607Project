/**
 * Light Struct
 * 
 * Fields:
 * position - vec4, position of the light. if w = 0 it's a directional light
 * color - vec3, color
 * ambient - float, ambient intensity
 * attenuation - float, attenuation of the light. only matters for non-directional lights
 * ambient_only - bool, if this light is only ambient light
 * 
 * Functions:
 * 
 * Use:
 * 0) Set MAX_LIGHTS in the fragment shader.
 * 1) Create a vector of lights in the main program
 * 2) Each time you're drawing something, call loadLights with the light vector and current shader.
 * 		- this fills in the uniform field of lights in the fragement shader for use, but also allows us to manipulate the lights by treating them as objects in the body of the main program (so we can move and modify them!)
**/

#include "glm/glm.hpp"

struct Light
{
	glm::vec4 position;
	glm::vec3 color;

	float ambient;
	float attenuation;

	bool ambient_only;
	// glm::vec3 diffuse;
	// glm::vec3 specular;

};