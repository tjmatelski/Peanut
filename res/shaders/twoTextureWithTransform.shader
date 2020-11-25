#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

uniform mat4 transform;

out vec3 ourColor;
out vec2 texCoord;

void main()
{
   gl_Position = transform * vec4(aPos, 1.0);
   ourColor = aColor;
   texCoord = aTex;
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
}