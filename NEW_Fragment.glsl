#version 150 core

#define MAX_LIGHTS 3

// Structs
struct FragLight {
  vec4 position;
	vec3 color;
	float ambient;
  float attenuation;
  bool ambient_only;
};

// Out/In
in vec3 Color;
in vec3 vertNormal;
in vec3 pos;
in vec2 texcoord;


out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform sampler2D tex6;
uniform sampler2D tex7;
uniform sampler2D tex8;
uniform sampler2D tex9;
uniform sampler2D tex10;
uniform sampler2D tex11;
uniform sampler2D tex12;

uniform int texID;
uniform mat4 view;

uniform FragLight inLights[MAX_LIGHTS];

vec4 applyLight(FragLight l, vec3 inColor)
{
  // if only ambient
  if (l.ambient_only)
  {
    return vec4(l.color*l.ambient*inColor, 1);
  }
  // get attenuation
  float attenuation = 1.0;
  if (l.position.w == 0.0)   // directional light
  {
    attenuation = 1.0;
  } else {                  // spot or point light
    vec3 LightDir_v = normalize(l.position.xyz - pos);
    float lightDist = length(LightDir_v); // from fragment to light
    attenuation = 1.0 /( 1.0 + l.attenuation * pow(lightDist, 2));

    // cone restrictions
    // float coneAngle = 20;
    // vec3 coneDirection = vec3(-l.position.xyz);
    // float angle = degrees(acos(dot(-LightDir_v, normalize(coneDirection))));
    // if(angle > coneAngle)
    // {
    //   attenuation = 0.0;
    // }
  }
  vec3 lightDir = (view * vec4(l.position.xyz,0.0)).xyz; // find light direction (its a vector!)
  vec3 normal = normalize(vertNormal);
  vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)! (Do you know why?)
  vec3 reflectDir = reflect(viewDir,normal);

  // ambient
  vec3 ambC = inColor*l.ambient*l.color; 

  // diffuse
  vec3 diffuseC = inColor*max(dot(-lightDir,normal),0.0)*l.color;

  // specular
  float spec = max(dot(reflectDir,lightDir),0.0);
  if (dot(-lightDir,normal) <= 0.0) {spec = 0; } //No highlight if we are not facing the light
  vec3 specC = .8*vec3(1.0,1.0,1.0)*pow(spec,4)*l.color;

  vec3 oColor = ambC+(diffuseC+specC)*attenuation;
  return vec4(oColor,1);;
}

void main() {

  vec3 color;

  // apply texture
  if (texID == -1)
    color = Color;
  else if (texID == 0)
    color = texture(tex0, texcoord).rgb;
  else if (texID == 1)
    color = texture(tex1, texcoord).rgb;  
  else if (texID == 2)
    color = texture(tex2, texcoord).rgb;  
  else if (texID == 3)
    color = texture(tex3, texcoord).rgb;  
  else if (texID == 4)
    color = texture(tex4, texcoord).rgb;  
  else if (texID == 5)
    color = texture(tex5, texcoord).rgb;  
  else if (texID == 6)
    color = texture(tex6, texcoord).rgb;  
  else if (texID == 7)
    color = texture(tex7, texcoord).rgb;  
  else if (texID == 8)
    color = texture(tex8, texcoord).rgb;  
  else if (texID == 9)
    color = texture(tex9, texcoord).rgb;  
  else if (texID == 10)
    color = texture(tex10, texcoord).rgb;  
  else if (texID == 11)
    color = texture(tex11, texcoord).rgb;  
  else if (texID == 12)
    color = texture(tex12, texcoord).rgb;
  else{
    outColor = vec4(1,0,0,1);
    return; //This was an error, stop lighting!
  }

  outColor = vec4(0.0f);
  for (int i = 0; i < MAX_LIGHTS; i++)
  {
    // to do - check if light is off, future feature
    outColor += outColor + applyLight(inLights[i], color);
  }
}
