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

#define M_PI 1
GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */


GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint quadrilateral_buffer; //vertex buffer object id for quadrilateral
GLuint X_axis_buffer;		 //vertex buffer object id for X_axis
GLuint Y_axis_buffer;		 //vertex buffer object id for Y_axis
GLuint Z_axis_buffer;		 //vertex buffer object id for Z_axis
GLuint sphere_buffer;		 //vertex buffer object id for sphere

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 12.0;
bool isMoving = false;		//Control whether the ball is moving or not
bool isStarted = false;		//indicate whether the moving is started or not
GLfloat radius;				//record the radius of circle
vec4 init_eye(7.0, 3.0, -10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position
mat4 accuMatrix	= Angel::identity();



//point3 route[] = { point3(-6.5, 1.0, -4.0), point3(3, 3, 3), point3(-2.0, 0, 3.0) };
point3 route[] = { point3(3.0, 1.0, 5.0), point3(-2.0, 1.0, -2.5), point3(2.0, 1.0, -4.0) };
int currentSegment = 0, totalSegments = 3;
GLfloat angle = 0.0, speed = 0.05;		//// rotation angle in degrees
point3 center = route[currentSegment];
point3 orientation[3];
point3 rotationAxis[3];


const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

//----------------------------------------------------------------------------
//there are col triangles and each triangle has three points	
static int col;

//----------------------------------------------------------------------------
//vertices of the floor 
point3 quadrilateral_points[4] = {
	point3(5.0, 0.0, 8.0),
	point3(5.0, 0.0, -4.0),
	point3(-5.0, 0.0, -4.0),
	point3(-5.0, 0.0, 8.0),
};

color3 quadrilateral_color = color3(0.0, 1.0, 0.0);
//----------------------------------------------------------------------------
//vertices of the X axis
point3 X_axis[3] = {
	point3(0.0, 0.0, 0.0),
	point3(10.0, 0.0, 0.0),
	point3(20.0, 0.0, 0.0),
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
point3 Y_axis[3] = {
	point3(0.0, 0.0, 0.0),
	point3(0.0, 10.0, 0.0),
	point3(0.0, 20.0, 0.0),
};
//color of the Y axis vertices 
color3 Y_axis_color[3] = {
	color3(1.0, 0.0, 1.0),
	color3(1.0, 0.0, 1.0),
	color3(1.0, 0.0, 1.0),
};
const int Y_axis_NumVertices = 3;
/*//----------------------------------------------------------------------------
//the sphere data structure
point3 sphereData[128 * 3];
color3 sphere_color[128 * 3];*/
point3 *sphereData;
color3 *sphere_color;
int sphere_NumVertices;
//----------------------------------------------------------------------------
//vertices of the Z axis
point3 Z_axis[3] = {
	point3(0.0, 0.0, 0.0),
	point3(0.0, 0.0, 10.0),
	point3(0.0, 0.0, 20.0),
};
//color of the Z axis vertices 
color3 Z_axis_color[3] = {
	color3(0.0, 0.0, 1.0),
	color3(0.0, 0.0, 1.0),
	color3(0.0, 0.0, 1.0),
};
const int Z_axis_NumVertices = 3;

//----------------------------------------------------------------------------
float calculateLength(float x, float y, float z) {
	return sqrt(x * x + y * y + z * z);
}
//----------------------------------------------------------------------------
void readFiles() {
	ifstream fp;
	string filename;

	int points;
	float temp[3];

	int count = 0;
	cout << "Please enter the filename. (1)sphere.8 (2)sphere.128" << endl;
	getline(cin, filename, '\n');
	fp.open(filename);
	if (!fp) {
		cerr << "Couldn't open " << filename << " !" << endl;
//		exit(3);
	}
	fp >> col;
	cout << col << endl;

	sphereData = new point3[col * 3];
	sphere_color = new point3[col * 3];

	for (int i = 0; i < col; i++) {
		fp >> points;
		for (int j = 0; j < points; j++) {
			for (int k = 0; k < 3; k++) {	//every point is defined as (x, y, z)
				fp >> temp[k];
			}
			sphereData[count++] = { temp[0], temp[1], temp[2] };
		}
	}

	sphere_NumVertices = col * 3;
	fp.close();
}



void colorsphere()
{
	for (int i = 0; i < col * 3; i++) {
		sphere_color[i] = color3(1.0, 0.84, 0.0);
	}

}
//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors
// the floor is generated by two triangles
void floor()
{
	floor_colors[0] = quadrilateral_color; floor_points[0] = quadrilateral_points[0];
	floor_colors[1] = quadrilateral_color; floor_points[1] = quadrilateral_points[1];
	floor_colors[2] = quadrilateral_color; floor_points[2] = quadrilateral_points[2];

	floor_colors[3] = quadrilateral_color; floor_points[3] = quadrilateral_points[0];
	floor_colors[4] = quadrilateral_color; floor_points[4] = quadrilateral_points[3];
	floor_colors[5] = quadrilateral_color; floor_points[5] = quadrilateral_points[2];
}

point3 calculateDirection(point3 from, point3 to) {
	point3 v;
	v.x = to.x - from.x;
	v.y = to.y - from.y;
	v.z = to.z - from.z;
	return normalize(v);
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

//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
	//set the sphere color
	colorsphere();

	//calculate radius
	radius = calculateRadius();
	
	//calculate the rolling directions
	for (int i = 0; i < totalSegments - 1; i++){
		orientation[i] = calculateDirection(route[i], route[i + 1]);
	}

	//and the last point to the first one
	orientation[totalSegments - 1] = calculateDirection(route[totalSegments - 1], route[0]);

	//calculate the rotating axis vector
	vec3 y_axis = { 0, 1, 0 };
	for (int i = 0; i < totalSegments; i++){
		rotationAxis[i] = cross(y_axis, orientation[i]);
	}
	floor();

	// Create and initialize a vertex buffer object for floor, to be used in display()
	glGenBuffers(1, &floor_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
		floor_colors);

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
	glBufferData(GL_ARRAY_BUFFER, 3 * col * sizeof(point3) + 3 * col * sizeof(color3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * col * sizeof(point3), sphereData);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * col * sizeof(point3), 3 * col * sizeof(color3),
		sphere_color);

	// Load shaders and create a shader program (to be used in display())
	program = InitShader("vshader42.glsl", "fshader42.glsl");


	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLineWidth(2.0);
}
//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj(GLuint buffer, int num_vertices)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/*----- Set up vertex attribute arrays for each vertex attribute -----*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point3) * num_vertices));
	// the offset is the (total) size of the previous vertex attribute array(s)

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
}
//----------------------------------------------------------------------------
void display(void)
{
	GLuint  model_view;  // model-view matrix uniform shader variable location
	GLuint  projection;  // projection matrix uniform shader variable location

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program); // Use the shader program

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	glClearColor(0.529, 0.807, 0.92, 0.0);
	/*---  Set up and pass on Projection matrix to the shader ---*/
	mat4  p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

	/*---  Set up and pass on Model-View matrix to the shader ---*/
	// eye is a global variable of vec4 set to init_eye and updated by keyboard()

	vec4    at(0.0, 0.0, 0.0, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);

	mat4  mv = LookAt(eye, at, up);


	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawObj(floor_buffer, floor_NumVertices);  // draw the floor

	//start to draw the X-axis
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(X_axis_buffer, X_axis_NumVertices);


	//start to draw the Y-axis
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(Y_axis_buffer, Y_axis_NumVertices);


	//start to draw the Z-axis
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(Z_axis_buffer, Z_axis_NumVertices);

	//start to draw the sphere

	mv = Translate(center.x, center.y, center.z)
		* LookAt(eye, at, up)
	    * Rotate(angle, rotationAxis[currentSegment].x, rotationAxis[currentSegment].y, rotationAxis[currentSegment].z)
		;
	//mv *= accuMatrix;
	//accuMatrix = mv;
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawObj(sphere_buffer, sphere_NumVertices);

	glutSwapBuffers();
}
//---------------------------------------------------------------------------

float distanceAt(point3 p1, point3 p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float dz = p1.z - p2.z;
	return calculateLength(dx, dy, dz);
}

int nextModel() {
	int next = currentSegment + 1;
	return (next == totalSegments) ? 0 : next;
}
bool isTrespass() {
	int next = nextModel();
	point3 from = route[currentSegment];
	point3 to = route[next];
	float d1 = distanceAt(center, from);
	float d2 = distanceAt(to, from);

	return d1 > d2;
}

//---------------------------------------------------------------------------
//animation
void idle(void)
{
//	angle += 0.02;
	// angle += 1.0;    //YJC: change this value to adjust the cube rotation speed.
	
	angle += speed;
	if (angle > 360.0)
		angle -= 360.0;

	float distance = (radius * speed * M_PI) / 180;
	center.x += orientation[currentSegment].x * distance;
	center.y += orientation[currentSegment].y * distance;
	center.z += orientation[currentSegment].z * distance;
	
	if (isTrespass()) {
		currentSegment = nextModel();
		center = route[currentSegment];
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
	case 'b': isStarted = true; glutIdleFunc(idle); break;
	case 'B': isStarted = true; glutIdleFunc(idle); break;

	case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
	case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
	case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

	}
	glutPostRedisplay();
}

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
	}
	glutPostRedisplay();
}


void Menu() {
	glutCreateMenu(menu);
	glutAddMenuEntry("Default View Point", 0);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_LEFT_BUTTON);
}


void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && isStarted) {
		isMoving = !isMoving;
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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	// glutInitContextVersion(3, 2);
	// glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Assignment 2");

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
	delete []sphereData;
	delete []sphere_color;
	return 0;
}
