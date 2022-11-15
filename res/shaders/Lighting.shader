#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   FragPos = vec3(model * vec4(aPos, 1.0));
   Normal = mat3(transpose(inverse(model))) *  aNormal; // TODO: inverse is very expensive. Switch to uniform eventually.
   TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

#define MAX_TEXTURES 16

struct Material {
    int numDiffuseTextures;
    sampler2D diffuse[MAX_TEXTURES];
    int numSpecularTextures;
    sampler2D specular[MAX_TEXTURES];
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    bool is_active;
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 8

uniform DirLight dirLight;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPos;

vec3 GetDiffuseTex()
{
    // TODO: Figure out how to combine texture values. Add? Multiply?
    return vec3(texture(material.diffuse[0], TexCoords));
}
vec3 GetSpecularTex()
{
    // TODO: Figure out how to combine texture values. Add? Multiply?
    return vec3(texture(material.specular[0], TexCoords));
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * GetDiffuseTex();
    vec3 diffuse  = light.diffuse  * diff * GetDiffuseTex();
    vec3 specular = light.specular * spec * GetSpecularTex();
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * GetDiffuseTex();
    vec3 diffuse  = light.diffuse  * diff * GetDiffuseTex();
    vec3 specular = light.specular * spec * GetSpecularTex();
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    if (theta > light.outerCutoff)
    {
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // combine results
        vec3 diffuse  = light.diffuse  * diff * GetDiffuseTex();
        vec3 specular = light.specular * spec * GetSpecularTex();

        // Calculate Intensity
        float epsilon = light.cutoff - light.outerCutoff;
        float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

        diffuse *= intensity;
        specular *= intensity;

        return (diffuse + specular);
    }
    else
    {
        return vec3(0.0);
    }
}

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        if (pointLights[i].is_active)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    }
    // phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}
