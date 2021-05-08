#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
const bool bloom = true;
const float exposure = 1.0;

void main()
{             

	const float gamma = 2.2;
	vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	if (bloomColor != vec3(0.0) && bloomColor != vec3(.2, 0.4, 0.8))
	{
		if(bloom)
				hdrColor += bloomColor; // additive blending
		// tone mapping
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		// also gamma correct while we're at it       
		result = pow(result, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
		// FragColor = vec4(bloomColor,1.0);
	} else {
		FragColor = vec4(hdrColor, 1.0);
		// FragColor = vec4(vec3(0.0), 1.0);
	}
}