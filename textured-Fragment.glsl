#version 150 core

in vec3 Color;
in vec3 vertNormal;
in vec3 pos;
in vec3 lightDir;
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

uniform vec3 lightColor;

uniform bool toon;

const float ambient = .3;
void main() {
  vec3 color;
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
  
  if (!toon)
  {
    vec3 normal = normalize(vertNormal);
    vec3 diffuseC = color*max(dot(-lightDir,normal),0.0);
    vec3 ambC = color*ambient;
    vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)! (Do you know why?)
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,lightDir),0.0);
    if (dot(-lightDir,normal) <= 0.0) spec = 0; //No highlight if we are not facing the light
    vec3 specC = .8*vec3(1.0,1.0,1.0)*pow(spec,4);
    vec3 oColor = (ambC+diffuseC+specC)*lightColor;
    outColor = vec4(oColor,1);

  } else {
    vec3 normal = normalize(vertNormal);
    vec3 viewDir = normalize(-pos);

    // diffuse
    vec3 diffuseC = vec3(max(dot(-lightDir,normal),0.0));
    float d_intensity = smoothstep(0, 0.01, diffuseC.x);

    // specular
    float gloss = 5.0;
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,lightDir),0.0);
    float s_intensity = smoothstep(0.005, 0.01, pow(spec * d_intensity, gloss * gloss));

    // rim light
    vec3 rimColor = vec3(1.0);
    float rimAmt = 0.7;
    float rimDot = 1.0 - dot(viewDir, normal);
    float r_intensity = smoothstep(rimAmt - 0.01, rimAmt +0.01, rimDot);

    vec3 oColor = (ambient + (lightColor*d_intensity) + s_intensity + (rimColor * r_intensity))*color*0.8;
    // vec3 oColor = (ambient + (lightColor*d_intensity) + s_intensity )*color*0.8 + (rimColor * r_intensity);

    outColor = vec4(oColor,1);
  }
  


  
}