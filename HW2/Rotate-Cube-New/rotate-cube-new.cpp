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

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint cube_buffer;   /* vertex buffer object id for cube */
GLuint floor_buffer;  /* vertex buffer object id for floor */

GLuint quadrilateral_buffer; //vertex buffer object id for quadrilateral
GLuint X_axis_buffer;		 //vertex buffer object id for X_axis
GLuint Y_axis_buffer;		 //vertex buffer object id for Y_axis
GLuint Z_axis_buffer;		 //vertex buffer object id for Z_axis

GLuint sphere_buffer;		 //vertex buffer object id for sphere

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 3.0;

GLfloat angle = 0.0; // rotation angle in degrees
//vec4 init_eye(3.0, 2.0, 0.0, 1.0); // initial viewer position
//vec4 eye = init_eye;               // current viewer position

vec4 init_eye(7.0, 3.0, -10.0, 1.0); // the VRP required  by the assignment
vec4 eye = init_eye;


int animationFlag = 1; // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'

int cubeFlag = 1;   // 1: solid cube; 0: wireframe cube. Toggled by key 'c' or 'C'
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'

const int cube_NumVertices = 36; //(6 faces)*(2 triangles/face)*(3 vertices/triangle)
#if 0
point3 cube_points[cube_NumVertices]; // positions for all vertices
color3 cube_colors[cube_NumVertices]; // colors for all vertices
#endif
#if 1
point3 cube_points[100]; 
color3 cube_colors[100];
#endif

const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

// Vertices of a unit cube centered at origin, sides aligned with axes
point3 vertices[8] = {
    point3( -0.5, -0.5,  0.5),
    point3( -0.5,  0.5,  0.5),
    point3(  0.5,  0.5,  0.5),
    point3(  0.5, -0.5,  0.5),
    point3( -0.5, -0.5, -0.5),
    point3( -0.5,  0.5, -0.5),
    point3(  0.5,  0.5, -0.5),
    point3(  0.5, -0.5, -0.5)
};

//vertices of the quadrilateral
point3 quadrilateral[4] = {
	point3(5.0, 0.0, 8.0),
	point3(5.0, 0.0, -4.0),
	point3(-5.0, 0.0, -4.0),
	point3(-5.0, 0.0, 8.0),
};
const int quadrilateral_NumVertices = 4;

//color of the quadrilateral
color3 quadrilateral_color[4] = {
	color3( 0.0, 1.0, 0.0),
	color3( 0.0, 1.0, 0.0),
	color3( 0.0, 1.0, 0.0),
	color3( 0.0, 1.0, 0.0),
};
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

color3 sphere_color = color3(1.0, 0.84, 0.0);
int col;		//record how many polygons it has
const int sphere_NumVertices = col * 3;		//there are col triangles and each triangle has three points	

// RGBA colors
color3 vertex_colors[8] = {
    color3( 0.0, 0.0, 0.0),  // black
    color3( 1.0, 0.0, 0.0),  // red
    color3( 1.0, 1.0, 0.0),  // yellow
    color3( 0.0, 1.0, 0.0),  // green
    color3( 0.0, 0.0, 1.0),  // blue
    color3( 1.0, 0.0, 1.0),  // magenta
    color3( 1.0, 1.0, 1.0),  // white
    color3( 0.0, 1.0, 1.0)   // cyan
};
//----------------------------------------------------------------------------
int Index = 0; // YJC: This must be a global variable since quad() is called
               //      multiple times and Index should then go up to 36 for
               //      the 36 vertices and colors


//----------------------------------------------------------------------------
// define the triangle class, each triangle has three vertices.
class triangle {
public:
	point3 vertices[3];

};

triangle *tri = new triangle[200];	//this has to be a global variable for later use

//----------------------------------------------------------------------------
// read the triangle from the text file
void readFiles() {
	ifstream read;	//ifstream reads files
	string filename;

	int points;		//record how many points it has
	float temp[3];

	int count = 0;

	cout << "Please enter the filename. (1)sphere.8 (2)sphere.128" << endl;
	getline(cin, filename, '\n');
	read.open(filename);
	if (!read) {
		cerr << "Error occured when opening files!" << endl;
		return;
	}
	read >> col;
	cout << col << endl;

	for (int i = 0; i < col; i++) {
		read >> points;
		for (int j = 0; j < points; j++) {
			for (int k = 0; k < 3; k++) {	//every point is defined as (x, y, z)
				read >> temp[k];
			}
			tri[i].vertices[j] = { temp[0], temp[1], temp[2] };
		}	
	}
	
	for (int i = 0; i < col; i++) {
		for (int j = 0; j < 3; j++)
			cout << tri[i].vertices[j];
		cout << endl;
	}
	

		

	
	read.close();

}

// quad(): generate two triangles for each face and assign colors to the vertices
void quad( int a, int b, int c, int d )
{
    cube_colors[Index] = vertex_colors[a]; cube_points[Index] = vertices[a]; Index++;
    cube_colors[Index] = vertex_colors[b]; cube_points[Index] = vertices[b]; Index++;
    cube_colors[Index] = vertex_colors[c]; cube_points[Index] = vertices[c]; Index++;

    cube_colors[Index] = vertex_colors[c]; cube_points[Index] = vertices[c]; Index++;
    cube_colors[Index] = vertex_colors[d]; cube_points[Index] = vertices[d]; Index++;
    cube_colors[Index] = vertex_colors[a]; cube_points[Index] = vertices[a]; Index++;
}
//----------------------------------------------------------------------------
// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}
//-------------------------------
// generate 2 triangles: 6 vertices and 6 colors
void floor()
{
    floor_colors[0] = vertex_colors[3]; floor_points[0] = vertices[3];
    floor_colors[1] = vertex_colors[0]; floor_points[1] = vertices[0];
    floor_colors[2] = vertex_colors[4]; floor_points[2] = vertices[4];

    floor_colors[3] = vertex_colors[4]; floor_points[3] = vertices[4];
    floor_colors[4] = vertex_colors[7]; floor_points[4] = vertices[7];
    floor_colors[5] = vertex_colors[3]; floor_points[5] = vertices[3];
}
//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
    colorcube();

#if 0 //YJC: The following is not needed
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
#endif

 // Create and initialize a vertex buffer object for cube, to be used in display()
    glGenBuffers(1, &cube_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_buffer);

#if 0
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points) + sizeof(cube_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_points), cube_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_points), sizeof(cube_colors),
                    cube_colors);
#endif
#if 1
    glBufferData(GL_ARRAY_BUFFER, sizeof(point3) * cube_NumVertices + sizeof(color3) * cube_NumVertices,
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * cube_NumVertices, cube_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point3) * cube_NumVertices, sizeof(color3) * cube_NumVertices,
                    cube_colors);
#endif

    floor();     
 // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
                    floor_colors);


// create and initialize a vertex buffer object for quadrilateral.
	glGenBuffers(1, &quadrilateral_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadrilateral_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadrilateral) + sizeof(quadrilateral_color),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadrilateral), quadrilateral);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(quadrilateral), sizeof(quadrilateral_color),
		quadrilateral_color);


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
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri) + sizeof(sphere_color),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tri), tri);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(tri), sizeof(sphere_color),
		sphere_color);


 // Load shaders and create a shader program (to be used in display())
    program = InitShader("vshader42.glsl", "fshader42.glsl");
    
    glEnable( GL_DEPTH_TEST );
	glClearColor(0.529, 0.807, 0.92, 0);
 //   glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
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
			  BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(sizeof(point3) * num_vertices) ); 
      // the offset is the (total) size of the previous vertex attribute array(s)

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
}
//----------------------------------------------------------------------------
void display( void )
{
  GLuint  model_view;  // model-view matrix uniform shader variable location
  GLuint  projection;  // projection matrix uniform shader variable location

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(program); // Use the shader program

    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );

    /*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

    /*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
//    vec4    at(0.0, 0.0, 0.0, 1.0);
//    vec4    up(0.0, 1.0, 0.0, 0.0);

//at = normalize(Eye + VPN) = normalize((7.0, 3.0, -10.0, 1.0) + (-7.0, -3.0, 10.0, 0)) = (0.0, 0.0, 0.0, 1.0)
//up = VUP = (0.0, 1.0, 0.0, 0.0)
	vec4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);
	mat4  mv = LookAt(eye, at, up);

#if 0 // The following is to verify the correctness of the function NormalMatrix():
      // Commenting out Rotate() and un-commenting mat4WithUpperLeftMat3() 
      // gives the same result.
    mv = Translate(-1.0, -0.5, 0.0) * mv * Scale (1.4, 1.4, 1.4)
            * Rotate(angle, 0.0, 0.0, 2.0); 
         // * mat4WithUpperLeftMat3(NormalMatrix(Rotate(angle, 0.0, 0.0, 2.0), 1));
#endif
#if 1 // The following is to verify that Rotate() about (0,2,0) is RotateY():
      // Commenting out Rotate() and un-commenting RotateY()
      // gives the same result.
    mv = Translate(-1.0, -0.5, 0.0) * mv * Scale (1.4, 1.4, 1.4) 
                    * Rotate(angle, 0.0, 2.0, 0.0);
		 // * RotateY(angle);
#endif
#if 0  // The following is to verify that Rotate() about (3,0,0) is RotateX():
       // Commenting out Rotate() and un-commenting RotateX()
       // gives the same result.
    mv = Translate(-1.0, -0.5, 0.0) * mv * Scale (1.4, 1.4, 1.4) 
                    * Rotate(angle, 3.0, 0.0, 0.0);
                 // * RotateX(angle);
#endif





    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    if (cubeFlag == 1) // Filled cube
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe cube
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(cube_buffer, cube_NumVertices);  // draw the cube

    mv = Translate(-1.0, -1.0, 0.3) * LookAt(eye, at, up) * Scale (1.6, 1.5, 3.3);
//	mv = Translate(-7.0, -3.0, 10.0) * LookAt(eye, at, up) * Scale(1.6, 1.5, 3.3);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawObj(floor_buffer, floor_NumVertices);  // draw the floor


//start to draw the quad
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawObj(quadrilateral_buffer, quadrilateral_NumVertices);

	
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
	

    glutSwapBuffers();
}
//---------------------------------------------------------------------------
void idle (void)
{
  angle += 0.02;
  // angle += 1.0;    //YJC: change this value to adjust the cube rotation speed.
  glutPostRedisplay();
}
//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;

        case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

        case 'a': case 'A': // Toggle between animation and non-animation
	    animationFlag = 1 -  animationFlag;
            if (animationFlag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
            break;
	   
        case 'c': case 'C': // Toggle between filled and wireframe cube
	    cubeFlag = 1 -  cubeFlag;   
            break;

        case 'f': case 'F': // Toggle between filled and wireframe floor
	    floorFlag = 1 -  floorFlag; 
            break;

	case ' ':  // reset to initial viewer/eye position
	    eye = init_eye;
	    break;
    }
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{ int err;


//call the readFiles function to read data from txt files.
	readFiles();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    // glutInitContextVersion(3, 2);
    // glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Color Cube");

  /* Call glewInit() and error checking */
  err = glewInit();
  if (GLEW_OK != err)
  { printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
    exit(1);
  }
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    init();


	

    glutMainLoop();
	delete tri;
	return 0;
}
