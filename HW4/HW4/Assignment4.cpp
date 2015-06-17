/************************************************************
* Handout: rotate-cube-new.cpp (A Sample Code for Shader-Based OpenGL ---
for OpenGL version 3.1 and later)
* Originally from Ed Angel's textbook "Interactive Computer Graphics" 6th Ed
sample code "example3.cpp" of Chapter 4.
* Moodified by Yi-Jen Chiang to include the use of a general rotation function
Rotate(angle, x, y, z), where the vector (x, y, z) can have length != 1.0,
and also to include the use of the function NormalMatrix(mv) to return the
normal matrix (mat3) of a given model-view matrix mv (mat4).

(The functions Rotate() and NormalMatrix() are added to the file "mat-yjc-new.h"
by Yi-Jen Chiang, where a new and correct transpose function "transpose1()" and
other related functions such as inverse(m) for the inverse of 3x3 matrix m are
also added; see the file "mat-yjc-new.h".)

* Extensively modified by Yi-Jen Chiang for the program structure and user
interactions. See the function keyboard() for the keyboard actions.
Also extensively re-structured by Yi-Jen Chiang to create and use the new
function drawObj() so that it is easier to draw multiple objects. Now a floor
and a rotating cube are drawn.

** Perspective view of a color cube using LookAt() and Perspective()

** Colors are assigned to each vertex and then the rasterizer interpolates
those colors across the triangles.
**************************************************************/
#include "Angel-yjc.h"
#include <fstream>
#include <string>


using namespace std;

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define M_PI 1
#define Particles_number 300
GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint program_Particles;       /* shader program object id */


GLuint firework_buffer;
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint floor_buffer_light;
GLuint floor_buffer_color;
GLuint X_axis_buffer;		 //vertex buffer object id for X_axis
GLuint Y_axis_buffer;		 //vertex buffer object id for Y_axis
GLuint Z_axis_buffer;		 //vertex buffer object id for Z_axis
GLuint sphere_buffer;		 //vertex buffer object id for sphere
GLuint sphere_buffer_smooth;
GLuint sphere_buffer_color;
GLuint shadow_buffer;		 //vertex buffer object id for sphere
GLuint floor_texture_buffer;
GLuint sphere_texture_buffer;		 //vertex buffer object id for sphere

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 13.0;
bool isMoving = false;		//Control whether the ball is moving or not
bool isStarted = false;		//indicate whether the moving is started or not
bool shadow = true;			//indicate whether produce the shadow, default is true
bool isSpotLight = true;
bool isPointLight = false;
bool wireframe = false;		//indicate whether produce wireframe mood or fill mood
bool isFlat = false;
bool isSmooth = true;
bool light = true;
int fog = 0;				//0: no fog;  1: linear;  2: exponential;  3: exponential square
bool blending = true;
int textureSphere = 1;          //0: no texture;  1: Contour Lines;  2: Checkerboard
int verticalTexture = 1;
int isEyeFrame = 0;
int is1D = 0;
int isUpRight = 1;
int isLattice = 1;
int isFloor = 1;
bool firework = true;
int textureFloor = 1;
GLfloat radius;				//record the radius of circle
vec4 init_eye(7.0, 3.0, -10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position


mat4 accuMatrix = Angel::identity();
vec4 lightSource(-14.0, 12.0, -3.0, 1.0);


color4 global_ambient(1.0, 1.0, 1.0, 1.0);
color4 light_ambient(0.0, 0.0, 0.0, 1.0);
color4 light_diffuse(0.8, 0.8, 0.8, 1.0);
color4 light_specular(0.2, 0.2, 0.2, 1.0);

//specify the distantant light direction
vec3 light_direction = normalize( vec3(0.1, 0.0, -1.0) );

//ground
color4 ground_diffuse(0.0, 1.0, 0.0, 1.0);
color4 ground_ambient(0.2, 0.2, 0.2, 1.0);
color4 ground_specular(0.0, 0.0, 0.0, 1.0);


//sphere
color4 sphere_diffuse(1.0, 0.84, 0.0, 1.0);
color4 sphere_ambient(0.2, 0.2, 0.2, 1.0);
color4 sphere_specular(1.0, 0.84, 0.0, 1.0);
float sphere_shininess = 125.0;

color4 ambient_ground_product = global_ambient * ground_ambient;
color4 diffuse_ground_product = light_diffuse * ground_diffuse;
color4 specular_ground_product = light_specular * ground_specular;

color4 ambient_sphere_product = global_ambient * sphere_ambient;
color4 diffuse_sphere_product = light_diffuse * sphere_diffuse;
color4 specular_sphere_product = light_specular * sphere_specular;

color4 positional_diffuse(1.0, 1.0, 1.0, 1.0);
color4 positional_specular(1.0, 1.0, 1.0, 1.0);
color4 positional_ambient(0.0, 0.0, 0.0, 1.0);

float constAtt = 2.0;
float linearAtt = 0.01;
float quadAtt = 0.001;

color4 positional_ambient_floor_product = positional_ambient * ground_ambient;
color4 positional_diffuse_floor_product = positional_diffuse * ground_diffuse;
color4 positional_specular_floor_product = positional_specular * ground_specular;

color4 positional_ambient_sphere_product = positional_ambient * sphere_ambient;
color4 positional_diffuse_sphere_product = positional_diffuse * sphere_diffuse;
color4 positional_specular_sphere_product = positional_specular * sphere_specular;

vec4 spotLightLocation(-6.0, 0.0, -4.5, 1.0);
vec4 spotLightDirection = normalize( spotLightLocation - lightSource );
float exponent = 15.0;
float cutoff = 20;



//fireworks settings
color3 particles_color[Particles_number];
point3 particles_location[Particles_number];
vec3 particles_velocity[Particles_number];
float time = 0.0;
float curTime = 0.0;
GLuint  model_view_firework;  // model-view matrix uniform shader variable location
GLuint  projection_firework;  // projection matrix uniform shader variable location
mat4 shadowMatrix = {12.0, 0.0, 0.0, 0.0,
					 14.0, 0.0, 3.0, -1.0,
					  0.0, 0.0, 12.0, 0.0,
					  0.0, 0.0, 0.0, 12.0};
/*12 14  0  0
   0  0  0  0
   0  3 12  0
   0 -1  0 12*/

//texture settings
#define ImageWidth  64
#define ImageHeight 64
GLubyte Image[ImageHeight][ImageWidth][4];

#define	stripeImageWidth 32
GLubyte stripeImage[4 * stripeImageWidth];
static GLuint texFloor;
static GLuint texSphere;
static GLuint texSphere_2D;



//--------------
point3 route[] = { point3(3.0, 1.0, 5.0), point3(-2.0, 1.0, -2.5), point3(2.0, 1.0, -4.0) };
int cur = 0, total = 3;
GLfloat angle = 0.0, speed = 0.1;		// rotation angle in degrees
point3 center = route[cur];
point3 orientation[3];
point3 ax[3];


const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point4 floor_points[floor_NumVertices]; // positions for all vertices
vec3 floor_normal[floor_NumVertices]; // colors for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

//----------------------------------------------------------------------------
//there are [col] triangles and each triangle has three points	
static int col;

//----------------------------------------------------------------------------
//vertices of the floor 
point4 quadrilateral_points[4] = {
	point4(5.0, 0.0, 8.0, 1.0),
	point4(5.0, 0.0, -4.0, 1.0),
	point4(-5.0, 0.0, -4.0, 1.0),
	point4(-5.0, 0.0, 8.0, 1.0),
};

vec3 quadrilateral_normal = vec3(0.0, 1.0, 0.0);
color3 quadrilateral_color = color3(0.0, 1.0, 0.0);

vec2 quad_texCoord[6] = {
	vec2(0.0, 0.0),  // for a
	vec2(0.0, 1.5),  // for b
	vec2(1.25, 1.5),  // for c

	vec2(0.0, 0.0),  // for a
	vec2(1.25, 0.0),  // for d
	vec2(1.25, 1.5),  // for c 
};

vec2 *sphere_texCoord;
//----------------------------------------------------------------------------
//vertices of the X axis
point4 X_axis[3] = {
	point4(0.0, 0.0, 0.0, 1.0),
	point4(10.0, 0.0, 0.0, 1.0),
	point4(20.0, 0.0, 0.0, 1.0),
};
//color of the X axis vertices 
color3 X_axis_color[3] = {
	color3(1.0, 0.0, 0.0),
	color3(1.0, 0.0, 0.0),
	color3(1.0, 0.0, 0.0),
};
const int X_axis_NumVertices = 3;
//----------------------------------------------------------------------------
//vertices of the Y axis
point4 Y_axis[3] = {
	point4(0.0, 0.0, 0.0, 1.0),
	point4(0.0, 10.0, 0.0, 1.0),
	point4(0.0, 20.0, 0.0, 1.0),
};
//color of the Y axis vertices 
color3 Y_axis_color[3] = {
	color3(1.0, 0.0, 1.0),
	color3(1.0, 0.0, 1.0),
	color3(1.0, 0.0, 1.0),
};
const int Y_axis_NumVertices = 3;

//----------------------------------------------------------------------------
//the sphere data structure
point4 *sphereData;
vec3 *sphere_normal;
vec3 *sphere_normal_smooth;
color3 *sphere_color;
int sphere_NumVertices;
point4 *sphere_normal_point;
vec3 *sphere_normal_new;
//----------------------------------------------------------------------------
//the sphere data structure

point4 *shadowData;
color3 *shadow_color;
int shadow_NumVertices;

//----------------------------------------------------------------------------
//vertices of the Z axis
point4 Z_axis[3] = {
	point4(0.0, 0.0, 0.0, 1.0),
	point4(0.0, 0.0, 10.0, 1.0),
	point4(0.0, 0.0, 20.0, 1.0),
};
//color of the Z axis vertices 
color3 Z_axis_color[3] = {
	color3(0.0, 0.0, 1.0),
	color3(0.0, 0.0, 1.0),
	color3(0.0, 0.0, 1.0),
};
const int Z_axis_NumVertices = 3;
/*************************************************************
void image_set_up(void):
generate checkerboard and stripe images.

* Inside init(), call this function and set up texture objects
for texture mapping.
(init() is called from main() before calling glutMainLoop().)
***************************************************************/
void image_set_up(void)
{
	int i, j, c;

	/* --- Generate checkerboard image to the image array ---*/
	for (i = 0; i < ImageHeight; i++)
		for (j = 0; j < ImageWidth; j++)
		{
		c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

		if (c == 1) /* white */
		{
			c = 255;
			Image[i][j][0] = (GLubyte)c;
			Image[i][j][1] = (GLubyte)c;
			Image[i][j][2] = (GLubyte)c;
		}
		else  /* green */
		{
			Image[i][j][0] = (GLubyte)0;
			Image[i][j][1] = (GLubyte)150;
			Image[i][j][2] = (GLubyte)0;

		}

		Image[i][j][3] = (GLubyte)255;
		}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*--- Generate 1D stripe image to array stripeImage[] ---*/
	for (j = 0; j < stripeImageWidth; j++) {
		/* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
		When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
		*/
		stripeImage[4 * j] = (GLubyte)255;
		stripeImage[4 * j + 1] = (GLubyte)((j>4) ? 255 : 0);
		stripeImage[4 * j + 2] = (GLubyte)0;
		stripeImage[4 * j + 3] = (GLubyte)255;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	/*----------- End 1D stripe image ----------------*/

	/*--- texture mapping set-up is to be done in
	init() (set up texture objects),
	display() (activate the texture object to be used, etc.)
	and in shaders.
	---*/

} /* end function */


void drawObj_firework(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	//GLuint vPosition = glGetAttribLocation(program, "vPosition");
	//glEnableVertexAttribArray(vPosition);
	//glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
	//	BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program_Particles, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));


	GLuint Velocity = glGetAttribLocation(program_Particles, "Velocity");
	glEnableVertexAttribArray(Velocity);
	glVertexAttribPointer(Velocity, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices));
//	 the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(Velocity);
	glDisableVertexAttribArray(vColor);
}




//----------------------------------------------------------------------------
float calculateLength(float x, float y, float z) {
	return sqrt(x * x + y * y + z * z);
}

//---------------------------------------------------------------------------
//calculate the radius of circle read from the text file
GLfloat calculateRadius() {
	GLfloat yMax = -100, yMin = 100;
	for (int i = 0; i < col * 3; i++) {
		if (yMax < sphereData[i].y) yMax = sphereData[i].y;
		if (yMin > sphereData[i].y) yMin = sphereData[i].y;
	}
	return (yMax - yMin) / 2;
}



void Setup_Particles() {
	
	for (int i = 0; i < Particles_number; i++) {
		particles_velocity[i].x = 2.0*((rand() % 256) / 256.0 - 0.5);
		particles_velocity[i].y = 1.2*2.0*((rand() % 256) / 256.0);
		particles_velocity[i].z = 2.0*((rand() % 256) / 256.0 - 0.5);
		particles_color[i].x = (rand() % 256) / 256.0;
		particles_color[i].y = (rand() % 256) / 256.0;
		particles_color[i].z = (rand() % 256) / 256.0;
		particles_location[i] = point3(0.0, 0.1, 0.0);
//		cout << particles_color[i] << endl;
	}
}
//----------------------------------------------------------------------
// SetUp_Lighting_Uniform_Vars(mat4 mv):
// Set up lighting parameters that are uniform variables in shader.
//
// Note: "LightPosition" in shader must be in the Eye Frame.
//       So we use parameter "mv", the model-view matrix, to transform
//       light_position to the Eye Frame.
//----------------------------------------------------------------------
void SetUp_Floor_Lighting_Uniform_Vars(mat4 mv)
{

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_ground_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_ground_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_ground_product);
	glUniform4fv(glGetUniformLocation(program, "positional_ambient_product"),
		1, positional_ambient_floor_product);
	glUniform4fv(glGetUniformLocation(program, "positional_diffuse_product"),
		1, positional_diffuse_floor_product);
	glUniform4fv(glGetUniformLocation(program, "positional_specular_product"),
		1, positional_specular_floor_product);

	//spot light settings
	if (isSpotLight == true) {
		glUniform1i(glGetUniformLocation(program, "isSpotLight"), 1);	
		glUniform1f(glGetUniformLocation(program, "exponent"),
			exponent);
		glUniform1f(glGetUniformLocation(program, "cutoff"),
			cutoff);
		vec4 spotLightDirection1 = mv * spotLightDirection;

		glUniform3fv(glGetUniformLocation(program, "spotLightDirection"),
			1, spotLightDirection1);
	}
	else 
		glUniform1i(glGetUniformLocation(program, "isSpotLight"), 0);

	glUniform1f(glGetUniformLocation(program, "ConstAtt"),
		constAtt);
	glUniform1f(glGetUniformLocation(program, "LinearAtt"),
		linearAtt);
	glUniform1f(glGetUniformLocation(program, "QuadAtt"),
		quadAtt);

	glUniform3fv(glGetUniformLocation(program, "light_direction"),
		1, light_direction);

	vec4 light_position_eyeFrame = mv * lightSource;
	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position_eyeFrame);
}

void SetUp_Sphere_Lighting_Uniform_Vars(mat4 mv)
{
	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_sphere_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_sphere_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_sphere_product);

	glUniform3fv(glGetUniformLocation(program, "light_direction"),
		1, light_direction);

	glUniform4fv(glGetUniformLocation(program, "positional_ambient_product"),
		1, positional_ambient_sphere_product);
	glUniform4fv(glGetUniformLocation(program, "positional_diffuse_product"),
		1, positional_diffuse_sphere_product);

	glUniform4fv(glGetUniformLocation(program, "positional_specular_product"),
		1, positional_specular_sphere_product);

	//spot light settings
	if (isSpotLight == true) {
		vec4 spotLightDirection1 = mv * spotLightDirection;
		glUniform1i(glGetUniformLocation(program, "isSpotLight"), 1);
		glUniform1f(glGetUniformLocation(program, "exponent"),
			exponent);
		glUniform1f(glGetUniformLocation(program, "cutoff"),
			cutoff);
		glUniform3fv(glGetUniformLocation(program, "spotLightDirection"),
			1, spotLightDirection1);
	}

	else
		glUniform1i(glGetUniformLocation(program, "isSpotLight"), 0);

	vec4 light_position_eyeFrame = mv * lightSource;
	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position_eyeFrame);
	glUniform1f(glGetUniformLocation(program, "Shininess"),
		sphere_shininess);
}



//----------------------------------------------------------------------------
void readFiles() {
	ifstream fp;
	string filename;

	int points;
	float temp[3];
	int count = 0;
	string option;
	cout << "Please enter the option: 1 for sphere.256, 2 for sphere.1024" << endl;
	getline(cin, option, '\n');

	if (option == "1") filename = "sphere.256";
	else if (option == "2") filename = "sphere.1024";
	else {
		cout << "Wrong input";
		exit(3);
	}

	fp.open(filename);
	if (!fp) {
		cerr << "Couldn't open " << filename << " !" << endl;
		//		exit(3);
	}
	fp >> col;
	cout << col << endl;

	sphereData = new point4[col * 3];
	sphere_color = new color3[col * 3];
//	sphere_normal = new vec3[col * 3];
	sphere_texCoord = new vec2[col * 3];

	shadowData = new point4[col * 3];
	shadow_color = new vec3[col * 3];
	for (int i = 0; i < col; i++) {
		fp >> points;
		for (int j = 0; j < points; j++) {
			for (int k = 0; k < 3; k++) {	//every point is defined as (x, y, z)
				fp >> temp[k];
			}
			sphereData[count] = { temp[0], temp[1], temp[2], 1 };
//			cout << sphereData[count] << endl;
			shadowData[count++] = { temp[0], temp[1], temp[2], 1 };
		}
	}
	count = 0;
	sphere_NumVertices = col * 3;
	shadow_NumVertices = col * 3;
	fp.close();
}

void colorsphere()
{
	for (int i = 0; i < col * 3; i++) {
		sphere_color[i] = color3(1.0, 0.84, 0.0);
		sphere_texCoord[i] = vec2(0.0, 0.0);
	}

}


//Add the normal vector
void light_sphere()
{
	sphere_normal = new vec3[col * 3];

	int count = 0;
	vec3 normal;
	point4 x, y, z;
	for (int i = 0; i < col; i++) {
		for (int j = 0; j < 3; j++) {
			if (j == 0) {
				x = sphereData[count++];
				continue;
			}
			if (j == 1) {
				y = sphereData[count++];
				continue;
			}
			if (j == 2) {
				z = sphereData[count++];
				continue;
			}
		}
		vec4 u = y - x;
		vec4 v = z - y;	
		vec4 w = x - z;
		normal = normalize( cross(u, v) );
		sphere_normal[count - 3] = normal;
		sphere_normal[count - 2] = normal;
		sphere_normal[count - 1] = normal;
		shadow_color[i] = vec3(0.25, 0.25, 0.25);
	}
	cout << count << endl;

}


//When smooth shading, the normals are the vectors from center to points on the sphere.
void calculateSmoothNormal() {
	sphere_normal_smooth = new vec3[col * 3];
	for (int i = 0; i < col * 3; i++) {
		sphere_normal_smooth[i].x = sphereData[i].x;
		sphere_normal_smooth[i].y = sphereData[i].y;
		sphere_normal_smooth[i].z = sphereData[i].z;
	}
}
//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors and 6 normals
// the floor is generated by two triangles
void floor()
{
	floor_normal[0] = quadrilateral_normal; floor_points[0] = quadrilateral_points[0]; floor_colors[0] = quadrilateral_color;
	floor_normal[1] = quadrilateral_normal; floor_points[1] = quadrilateral_points[1]; floor_colors[1] = quadrilateral_color;
	floor_normal[2] = quadrilateral_normal; floor_points[2] = quadrilateral_points[2]; floor_colors[2] = quadrilateral_color;

	floor_normal[3] = quadrilateral_normal; floor_points[3] = quadrilateral_points[0]; floor_colors[3] = quadrilateral_color;
	floor_normal[4] = quadrilateral_normal; floor_points[4] = quadrilateral_points[3]; floor_colors[4] = quadrilateral_color;
	floor_normal[5] = quadrilateral_normal; floor_points[5] = quadrilateral_points[2]; floor_colors[5] = quadrilateral_color;
}

point3 calculateDirection(point3 from, point3 to) {
	point3 v;
	v.x = to.x - from.x;
	v.y = to.y - from.y;
	v.z = to.z - from.z;
	return normalize(v);
}



//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
	//set the sphere color
	light_sphere();
	colorsphere();
	image_set_up();
	calculateSmoothNormal();
	//calculate radius
	radius = calculateRadius();

	//calculate the rolling directions
	for (int i = 0; i < total - 1; i++){
		orientation[i] = calculateDirection(route[i], route[i + 1]);
	}

	//and the last point to the first one
	orientation[total - 1] = calculateDirection(route[total - 1], route[0]);

	//calculate the rotating axis vector
	vec3 y_axis = { 0, 1, 0 };
	for (int i = 0; i < total; i++){
		ax[i] = cross(y_axis, orientation[i]);
	}
	floor();


	// Create and initialize a vertex buffer object for floor, to be used in display()
	glGenBuffers(1, &floor_buffer_light);
	glBindBuffer(GL_ARRAY_BUFFER, floor_buffer_light);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_normal),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_normal),
		floor_normal);

	glGenBuffers(1, &floor_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_normal) + sizeof(quad_texCoord),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_normal),
		floor_normal);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_normal), sizeof(quad_texCoord),
		quad_texCoord);

	// create and initialize a vertex buffer object for X-axis.
	glGenBuffers(1, &X_axis_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, X_axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(X_axis) + sizeof(X_axis_color),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(X_axis), X_axis);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(X_axis), sizeof(X_axis_color),
		X_axis_color);


	// create and initialize a vertex buffer object for Y-axis.
	glGenBuffers(1, &Y_axis_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Y_axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Y_axis) + sizeof(Y_axis_color),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Y_axis), Y_axis);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Y_axis), sizeof(Y_axis_color),
		Y_axis_color);


	// create and initialize a vertex buffer object for Z-axis.
	glGenBuffers(1, &Z_axis_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Z_axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Z_axis) + sizeof(Z_axis_color),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Z_axis), Z_axis);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Z_axis), sizeof(Z_axis_color),
		Z_axis_color);


	// create and initialize a vertex buffer object for sphere.
	glGenBuffers(1, &sphere_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4) + 3 * col * sizeof(vec3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * col * sizeof(point4), sphereData);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4), 3 * col * sizeof(vec3),
		sphere_normal);


	// create and initialize a vertex buffer object for smooth sphere.
	glGenBuffers(1, &sphere_buffer_smooth);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer_smooth);
	glBufferData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4) + 3 * col * sizeof(vec3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * col * sizeof(point4), sphereData);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4), 3 * col * sizeof(vec3),
		sphere_normal_smooth);

	// create and initialize a vertex buffer object for shadow.
	glGenBuffers(1, &shadow_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4) + 3 * col * sizeof(color3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * col * sizeof(point4), shadowData);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4), 3 * col * sizeof(color3),
		shadow_color);
	

	// create the vertex sphere buffer for drawing when light is disable
	glGenBuffers(1, &sphere_buffer_color);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer_color);
	glBufferData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4) + 3 * col * sizeof(color3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * col * sizeof(point4), sphereData);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4), 3 * col * sizeof(color3),
		sphere_color);

	// create the vertex floor buffer for drawing when light is disable
	glGenBuffers(1, &floor_buffer_color);
	glBindBuffer(GL_ARRAY_BUFFER, floor_buffer_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
		floor_colors);

	// create the vertex firework buffer
	glGenBuffers(1, &firework_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, firework_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particles_color) + sizeof(particles_velocity),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particles_color), particles_color);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(particles_color), sizeof(particles_velocity),
		particles_velocity);


	//create the floor texture buffer
	/*--- Create and Initialize a texture object ---*/
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texFloor);      // Generate texture obj name(s)

	glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
	glBindTexture(GL_TEXTURE_2D, texFloor); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

	/*--- Create and initialize vertex buffer object for quad ---*/
	glGenBuffers(1, &floor_texture_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, floor_texture_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(quad_texCoord),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points),
		sizeof(quad_texCoord), quad_texCoord);


	/*--- Create and initialize vertex buffer object for quad ---*/
	glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 1 
	glBindTexture(GL_TEXTURE_1D, texSphere); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);



	glActiveTexture(GL_TEXTURE2);  // Set the active texture unit to be 0 
	glBindTexture(GL_TEXTURE_2D, texSphere_2D); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

	//glGenBuffers(1, &sphere_texture_buffer);
	//glBindBuffer(GL_ARRAY_BUFFER, sphere_texture_buffer);
	//glBufferData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4) + 3 * col * sizeof(vec2),
	//	NULL, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * col * sizeof(point4), sphereData);
	//glBufferSubData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4),
	//	3 * col * sizeof(vec2), sphere_texCoord);

	glGenBuffers(1, &sphere_texture_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_texture_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * col * sizeof(point4),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * col * sizeof(point4), sphereData);


	// Load shaders and create a shader program (to be used in display())
	program = InitShader("vshader53.glsl", "fshader53.glsl");
	program_Particles = InitShader("vshaderParticles.glsl", "fshaderParticles.glsl");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLineWidth(2.0);
}


float distanceFromTo(point3 p1, point3 p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float dz = p1.z - p2.z;
	return calculateLength(dx, dy, dz);
}

//---------------------------------------------------------------------------

int nextIndex() {
	int next = cur + 1;
	return (next == total) ? 0 : next;
}

//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj_floor(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4) * num_vertices));


	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(num_vertices * sizeof(point4) + num_vertices * sizeof(vec3)));
	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);
	glDisableVertexAttribArray(vTexCoord);
}

//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj_color(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4) * num_vertices));
	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
}


//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj_light(GLuint buffer, int num_vertices)
{

	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));
	
	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4) * num_vertices));
	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vNormal);
}

bool isTrespass() {
	int next = nextIndex();
	point3 from = route[cur];
	point3 to = route[next];
	float d1 = distanceFromTo(center, from);
	float d2 = distanceFromTo(to, from);
	return d1 > d2;
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLuint  model_view;  // model-view matrix uniform shader variable location
	GLuint  projection;  // projection matrix uniform shader variable location

	//** Important: glUseProgram() must be called *before* any shader variable
	//              locations can be retrieved. This is needed to pass on values to
	//              uniform/attribute variables in shader ("variable binding" in 
	//              shader).
	glUseProgram(program);
	glClearColor(0.529, 0.807, 0.92, 0.0);
	// Retrieve transformation uniform variable locations
	// ** Must be called *after* glUseProgram().
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	
	/*---  Set up and pass on Projection matrix to the shader ---*/
	mat4  p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major


	vec4    at(0.0, 0.0, 0.0, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);
	mat4	mv = LookAt(eye, at, up);
	mat4	mvShadow;
	mat4	eyeFrame = LookAt(eye, at, up);
	accuMatrix = Rotate(speed, ax[cur].x, ax[cur].y, ax[cur].z) * accuMatrix;
	mvShadow = accuMatrix;
	mv = LookAt(eye, at, up) * Translate(center.x, center.y, center.z)
			* accuMatrix;

	mat3 normal_matrix_floor = NormalMatrix(eyeFrame, 1);

	glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
	glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
	
	glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
		1, GL_TRUE, normal_matrix_floor);
	glUniform1i(glGetUniformLocation(program, "fog"), fog);
	glUniform1i(glGetUniformLocation(program, "isFloor"), 1);
	//disable writing to z-buffer, enable writing to frame buffer
	glDepthMask(GL_FALSE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//start to draw the floor
	SetUp_Floor_Lighting_Uniform_Vars(eyeFrame);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeFrame); // GL_TRUE: matrix is row-major
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (light == true) {
		glUniform1i(glGetUniformLocation(program, "isShading"), 1);	//switch to shading drawing
		if (textureFloor == 1) drawObj_floor(floor_buffer, floor_NumVertices);
		else {
			glUniform1i(glGetUniformLocation(program, "isFloor"), 0);
			glUniform1i(glGetUniformLocation(program, "isTexture"), 0);
			drawObj_light(floor_buffer_light, floor_NumVertices);
		}
	}
	else {
		glUniform1i(glGetUniformLocation(program, "isShading"), 0);

		drawObj_color(floor_buffer_color, floor_NumVertices);		
	}


	if (blending == true) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	//draw the shadow, with the z-buffer still disabled.
	//Combine the shadow matrix with the rolling ball
	glUniform1i(glGetUniformLocation(program, "isFloor"), 0);
	glUniform1i(glGetUniformLocation(program, "isShading"), 0);
	glUniform1i(glGetUniformLocation(program, "isShadow"), 1);
	if (shadow == true) {
		mvShadow = LookAt(eye, at, up) * shadowMatrix * Translate(center.x, center.y, center.z) * accuMatrix;
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mvShadow);
		if (wireframe == true)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (eye[1] >= 0) drawObj_color(shadow_buffer, shadow_NumVertices);
	}
	glDisable(GL_BLEND);
	//restore the ground in the z-buffer
	//disable writing to frame buffer, enable writing to z-buffer
	glUniform1i(glGetUniformLocation(program, "isShading"), 1);
	glUniform1i(glGetUniformLocation(program, "isShadow"), 0);
	glDepthMask(GL_TRUE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
	//start to draw the floor
	glUniform1i(glGetUniformLocation(program, "isFloor"), 1);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeFrame); // GL_TRUE: matrix is row-major
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (light == true) {
		if (textureFloor == 1) drawObj_floor(floor_buffer, floor_NumVertices);
		else {
			glUniform1i(glGetUniformLocation(program, "isFloor"), 0);
			glUniform1i(glGetUniformLocation(program, "isTexture"), 0);
			drawObj_light(floor_buffer_light, floor_NumVertices);
			
		}
	}
	else {
		glUniform1i(glGetUniformLocation(program, "isShading"), 0);
		drawObj_color(floor_buffer_color, floor_NumVertices);
	}


	//resume the normal operations
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	
	glUniform1i(glGetUniformLocation(program, "isFloor"), 0);
	glUniform1i(glGetUniformLocation(program, "isShading"), 0);
	glUniform1i(glGetUniformLocation(program, "isTexture"), 0);
	//start to draw the X-axis
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeFrame);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj_color(X_axis_buffer, X_axis_NumVertices);

	//start to draw the Y-axis
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeFrame);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj_color(Y_axis_buffer, Y_axis_NumVertices);

	//start to draw the Z-axis
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeFrame);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj_color(Z_axis_buffer, Z_axis_NumVertices);
	
	
	//draw the sphere
	glUniform1i(glGetUniformLocation(program, "isTexture"), textureSphere);
	glUniform1i(glGetUniformLocation(program, "isVertical"), verticalTexture);
	glUniform1i(glGetUniformLocation(program, "isEyeFrame"), isEyeFrame);
	glUniform1i(glGetUniformLocation(program, "isLattice"), isLattice);
	glUniform1i(glGetUniformLocation(program, "isUpRight"), isUpRight);
	
	if (is1D == 1){
		glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
		glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);
		glUniform1i(glGetUniformLocation(program, "is1D"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(program, "is1D"), 0);
		glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
		glUniform1i(glGetUniformLocation(program, "texture_2D"), 2);
	}


	
	mat3 normal_matrix_sphere = NormalMatrix(mv, 1);
	glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
		1, GL_TRUE, normal_matrix_sphere);
	glUniform1i(glGetUniformLocation(program, "isShading"), 1);
	SetUp_Sphere_Lighting_Uniform_Vars(eyeFrame);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	if (wireframe == true) {
		glUniform1i(glGetUniformLocation(program, "isTexture"), 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (light == true) {
		if (isFlat)
			drawObj_light(sphere_buffer, sphere_NumVertices);
		if (isSmooth)
			drawObj_light(sphere_buffer_smooth, sphere_NumVertices);
	}	
	else {
		glUniform1i(glGetUniformLocation(program, "isShading"), 0);
		drawObj_color(sphere_buffer_color, sphere_NumVertices);
	}
	
	//draw the firework
	GLuint  model_view_firework;  // model-view matrix uniform shader variable location
	GLuint  projection_firework;  // projection matrix uniform shader variable location
	time = (float)glutGet(GLUT_ELAPSED_TIME) - curTime;
	time = (int)time % 7000;

	glUseProgram(program_Particles);
	model_view_firework = glGetUniformLocation(program_Particles, "model_view");
	projection_firework = glGetUniformLocation(program_Particles, "projection");
	glUniformMatrix4fv(projection_firework, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
	glUniformMatrix4fv(model_view_firework, 1, GL_TRUE, eyeFrame); // GL_TRUE: matrix is row-major
	glUniform1f(glGetUniformLocation(program_Particles, "time"), time);
	
	glPointSize(3);

	if (firework == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		drawObj_firework(firework_buffer, Particles_number);
	}

	glutSwapBuffers();
}

void idle(void)
{
	angle += speed;
	if (angle > 360.0)
		angle -= 360.0;

	float distance = (radius * speed * M_PI) / 180;
	center.x += orientation[cur].x * distance;
	center.y += orientation[cur].y * distance;
	center.z += orientation[cur].z * distance;

	if (isTrespass()) {
		cur = nextIndex();
		center = route[cur];
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
//keyboard control
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

		//after pressed the b or B button, we can control the moving
	case 'b': isStarted = true; glutIdleFunc(idle); curTime = (float)glutGet(GLUT_ELAPSED_TIME); break;
	case 'B': isStarted = true; glutIdleFunc(idle); curTime = (float)glutGet(GLUT_ELAPSED_TIME); break;

	case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
	case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
	case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

	case 'v': verticalTexture = 1; break;
	case 'V': verticalTexture = 1; break;
	case 's': verticalTexture = 0; break;
	case 'S': verticalTexture = 0; break;
	case 'o': isEyeFrame = 1; break;
	case 'O': isEyeFrame = 1; break;
	case 'e': isEyeFrame = 0; break;
	case 'E': isEyeFrame = 0; break;
	case 'l': isLattice = 1 - isLattice; break;
	case 'L': isLattice = 1 - isLattice; break;
	case 'u': isUpRight = 1; break;
	case 'U': isUpRight = 1; break;
	case 't': isUpRight = 0; break;
	case 'T': isUpRight = 0; break;

	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
//menu options
void menu(int id) {
	switch (id) {
		case(0) : {
			eye = init_eye;
			break;
		}
		case(1) : {
			exit(3);
			break;
		}
		case(2) : {
			shadow = false;
			break;
		}
		case(3) : {
			shadow = true;
			break;
		}
		case(4) : {
			light = false;
			break;
		}
		case(5) : {
			light = true;
			break;
		}
		case(6) : {
			wireframe = !wireframe;
			break;
		}
		case(7) : {
			isFlat = true;
			isSmooth = false;
			break;
		}
		case(8) : {
			isFlat = false;
			isSmooth = true;
			break;
		}
		case(9) : {
			isSpotLight = true;
			isPointLight = false;
			break;
		}
		case(10) : {
			isSpotLight = false;
			isPointLight = true;
			break;
		}
		case(11) : {
			fog = 0;
			break;
		}
		case(12) : {
			fog = 1;
			break;
		}
		case(13) : {
			fog = 2;
			break;
		}
		case(14) : {
			fog = 3;
			break;
		}
		case(15) : {
			blending = false;
			break;
		}
		case(16) : {
			blending = true;
			break;
		}
		case(17) : {
			textureFloor = 0;
			break;
		}
		case(18) : {
			textureFloor = 1;
			break;
		}
		case(19) : {
			textureSphere = 0;
			break;
		}
		case(20) : {
			is1D = 1;
			break;
		}
		case(21) : {
			is1D = 0;
			break;
		}
		case(22) : {
			firework = false;
			break;
		}
		case(23) : {
			firework = true;
			curTime = (float)glutGet(GLUT_ELAPSED_TIME);
			break;
		}
	}
	glutPostRedisplay();
}
//----------------------------------------------------------------------------
//add menu
void Menu() {
	//add the shadow submenu
	int shadow = glutCreateMenu(menu);
	glutAddMenuEntry("No", 2);
	glutAddMenuEntry("Yes", 3);

	//add the lighting submenu
	int lighting = glutCreateMenu(menu);
	glutAddMenuEntry("No", 4);
	glutAddMenuEntry("Yes", 5);

	//add the shading submenu
	int shading = glutCreateMenu(menu);
	glutAddMenuEntry("flat shading", 7);
	glutAddMenuEntry("smooth shading", 8);

	//add the lightsource submenu
	int lightSource = glutCreateMenu(menu);
	glutAddMenuEntry("spot light", 9);
	glutAddMenuEntry("point source", 10);

	//add the fog options submenu
	int fog = glutCreateMenu(menu);
	glutAddMenuEntry("no fog", 11);
	glutAddMenuEntry("linear", 12);
	glutAddMenuEntry("exponential", 13);
	glutAddMenuEntry("exponential square", 14);

	//add the Blending shadow submenu
	int blending = glutCreateMenu(menu);
	glutAddMenuEntry("No", 15);
	glutAddMenuEntry("Yes", 16);

	//add the texture ground submenu
	int texGround = glutCreateMenu(menu);
	glutAddMenuEntry("No", 17);
	glutAddMenuEntry("Yes", 18);

	//add the texture sphere submenu
	int texSphere = glutCreateMenu(menu);
	glutAddMenuEntry("No", 19);
	glutAddMenuEntry("Yes - Contour Lines",20);
	glutAddMenuEntry("Yes - Checkerboard", 21);

	//add the firework submenu
	int firework = glutCreateMenu(menu);
	glutAddMenuEntry("No", 22);
	glutAddMenuEntry("Yes", 23);


	//create the main menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Default View Point", 0);
	glutAddMenuEntry("Quit", 1);
	glutAddSubMenu("Shadow", shadow);
	glutAddSubMenu("Enable Lighting", lighting);
	glutAddMenuEntry("Wire Frame Sphere", 6);
	glutAddSubMenu("Shading", shading);
	glutAddSubMenu("Light Source", lightSource);
	glutAddSubMenu("Fog Options", fog);
	glutAddSubMenu("Blending Shadow", blending);
	glutAddSubMenu("Texture Mapped Ground", texGround);
	glutAddSubMenu("Texture Mapped Sphere", texSphere);
	glutAddSubMenu("Firework", firework);
	glutAttachMenu(GLUT_LEFT_BUTTON);
}


void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && isStarted) {
		isMoving = !isMoving;
//		curTime = (float)glutGet(GLUT_ELAPSED_TIME);
		if (isMoving) glutIdleFunc(idle);
		else glutIdleFunc(NULL);
	}
}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	aspect = (GLfloat)width / (GLfloat)height;
	glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int err;
	readFiles();
	Setup_Particles();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//Enable the depth buffer
	glEnable(GL_DEPTH_TEST);
	glutInitWindowSize(512, 512);
	// glutInitContextVersion(3, 2);
	// glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Assignment 4");

	/* Call glewInit() and error checking */
	err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
		exit(1);
	}

	Menu();//add menu
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);

	glutKeyboardFunc(keyboard);

	init();
	glutMainLoop();

	//clean up
	delete[]sphereData;
	delete[]sphere_normal;
	delete[]sphere_color;
	delete[]shadowData;
	delete[]shadow_color;
	delete[]sphere_texCoord;
	return 0;
}
