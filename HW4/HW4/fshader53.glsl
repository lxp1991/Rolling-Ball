/* 
File Name: "fshader53.glsl":
           Fragment Shader
*/

// #version 150 


in  vec4 color;
//The position of vertex
in vec3 Vpos;
out vec4 fColor;

uniform int fog;	
uniform int is1D;


//texture settings
uniform sampler2D texture_2D; /* Note: If using multiple textures,
                                       each texture must be bound to a
                                       *different texture unit*, with the
                                       sampler uniform var set accordingly.
                                 The (fragment) shader can access *all texture units*
                                 simultaneously.
                              */
uniform sampler1D texture_1D;


in  vec2 texCoord;
in float texCoord1D;
uniform int isTexture;
uniform int isLattice;
varying float s_lattice;
varying float t_lattice;
uniform int isFloor;


void main() 
{ 
	vec4 fogColor = vec4(0.7, 0.7, 0.7, 0.5);
	float fStart = 0.0;
	float fEnd = 18.0;
	float z = length(Vpos);
	z = clamp(z, fStart, fEnd);
	float density = 0.09;

	float f_linear = (fEnd - z) / (fEnd - fStart);
	float f_exp = exp(-(density * z));
	float f_exp_2 = exp(-(density * z)*(density * z));

	float coeff;

	if (fog == 0) coeff = 1;
	if (fog == 1) coeff = f_linear;
	if (fog == 2) coeff = f_exp;
	if (fog == 3) coeff = f_exp_2;
	

	if (isFloor == 1) fColor = color * texture(texture_2D, texCoord);
	else {
	if (isTexture == 1)
		if (is1D == 1) { 
			if (isLattice == 1) if (0 < fract(4 * s_lattice) &&  0.35 > fract(4 * s_lattice) && 0 < fract(4 * t_lattice) &&  0.35 > fract(4 * t_lattice)) discard;
			fColor = color * texture( texture_1D, texCoord1D );
			}
		else {
			if (isLattice == 1) if (0 < fract(4 * s_lattice) &&  0.35 > fract(4 * s_lattice) && 0 < fract(4 * t_lattice) &&  0.35 > fract(4 * t_lattice)) discard;
			vec4 temp = texture( texture_2D, texCoord );
			fColor = color * temp;
			if (temp.x == 0.0	)
				fColor = color * vec4(0.9, 0.1, 0.1, 1.0);	
		}
	else 
		fColor = color;
	}

	fColor = mix (fogColor, fColor, coeff);
} 

