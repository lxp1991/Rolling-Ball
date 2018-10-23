/* 
File Name: "vshader53.glsl":
Vertex shader:
  - Per vertex shading for a single point light source;
    distance attenuation is Yet To Be Completed.
  - Entire shading computation is done in the Eye Frame.
*/

// #version 150 

//in  vec3 vPosition;
//add the parameters
in  vec3 vColor;
in  vec3 Velocity;
out vec4 color;


uniform float time;
uniform mat4 model_view;
uniform mat4 projection;


varying float yPosition;

void main()
{
   
   vec4 curPosition;
   
   curPosition.x = 0.001 * Velocity.x * time;
   curPosition.y = 0.1 + 0.001 * Velocity.y * time + 0.5* (-0.00000049) * time * time;
   curPosition.z = 0.001 * Velocity.z * time;
   curPosition.w = 1.0;

   yPosition = curPosition.y;

    gl_Position = projection * model_view * curPosition, 1.0;

	vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, 1.0); 
	color = vColor4;
	

}
