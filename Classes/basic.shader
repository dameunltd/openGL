#shader vertex
#version 330 core

in vec4 position;

void main()
{
   gl_Position = position;
}

#shader fragment
#version 330 core

out vec4 color;

/* type of variable = vec4; u_Color tells you that it's a uniform */
uniform vec4 u_Color;

void main()
{
    color = u_Color;
}
