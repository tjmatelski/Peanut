#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;

out vec3 fragColor;
out vec2 texCoord;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   fragColor = color;
   texCoord = aTex;
}

#shader fragment
#version 330 core

in vec3 fragColor;
in vec2 texCoord;

uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
    FragColor = texture(texture1, texCoord) * vec4(fragColor, 1.0);
}