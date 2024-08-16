#version 400

in vec2 tC;
uniform sampler2D diffuse;

out vec4 FragColour;

void main()
{
	 FragColour = texture2D(diffuse, tC);
}

