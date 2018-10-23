/* 
File Name: "fshaderParticles.glsl":
           Fragment Shader
*/

// #version 150 


in  vec4 color;
out vec4 fColor;

varying float yPosition;

void main() 
{ 

	if (yPosition <= 0.1) discard;
	fColor = color;

} 

