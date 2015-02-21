#define WINDOWS     1 /* Set to 1 for Windows, 0 else */
#define UNIX_LINUX  0 /* Set to 1 for Unix/Linux, 0 else */

#if WINDOWS
#include <windows.h>
#include <GL/glut.h>
#endif
#if UNIX_LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define XOFF          50
#define YOFF          50
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
#define MAXLINE 20



void display(void);
void myinit(void);
void draw_circle(int x, int y, int r);
void draw_arc(int x, int y, int r, int xd, int yd);
/* Function to handle file input; modification may be needed */
void file_in(void);
void idle(void);
void chooseOption(void);

int x, y, r;

int option;	
int rows;

float K = 10000;
float frame = 1;


int data[MAXLINE][MAXLINE];

/*-----------------
The main function
------------------*/
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

//read from the text file.
	file_in();


	chooseOption();

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	/* Use both double buffering and Z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutIdleFunc(idle);       // Register our idle() function
	glutInitWindowPosition(XOFF, YOFF);

	glutCreateWindow("CS6533 Assignment 1");
	glutDisplayFunc(display);



	myinit();
	glutMainLoop();
}

/*---------------------------------------------------------------------
chooseOption(): This function is called once for _every_ frame.
---------------------------------------------------------------------*/
void chooseOption(void)
{
	cout << "There 3 options for this program.\n";
	cout << "Enter 1 to draw a circle (The center and radius are given by you.)\n";
	cout << "Enter 2 to draw serverl circles (Arguements are given by a txt file)\n";
	cout << "Enter 3 to draw growing circles\n";
	cout << "Please enter the option index:\n";
	cin >> option;

	if (option == 1)
	{
		cout << "Please enter x, y, r\n";
		cin >> x;
		cin >> y;
		cin >> r;
	}
}

/*----------
file_in(): file input function. Modify here.
------------*/
void file_in(void)
{
	errno_t err;
	FILE *stream;

	int maxX = 0;//store the maximum X value of the circles can reach
	int maxY = 0;//store the maximum Y value of the circles can reach
	int maxCoord;//Max value of maxX and maxY
	char *buf; // Store the first row of the circle file.

	//Since the window may not be large enough to hold all circles, we have to multiply all circles by a uniform scaling factor 
	float scalingFactor;

	//read data from the circles.txt file.
	if ((err = fopen_s(&stream, "circles.txt", "r")) != 0)
		printf("The file 'circles.txt' was not opened\n");
	else
		printf("The file 'circles.txt' was opened\n");

	buf = (char *)malloc(sizeof(char *));
	fgets(buf, 10, stream);

	rows = atoi(buf);//Number of circles in the txt file.
	buf = NULL;

	//Read x, y and radius and store them into data[][]
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < 3; j++)
			fscanf_s(stream, "%d", &data[i][j]);
		
	//close the link between the file
	fclose(stream);


	//calculate the maxX and maxY value 
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (j == 0)
				if (maxX < (data[i][0] + data[i][2]) || maxX < abs(data[i][0] - data[i][2])) maxX = max(data[i][0] + data[i][2], abs(data[i][0] - data[i][2]));
			if (j == 1)
				if (maxY < (data[i][1] + data[i][2]) || maxY < abs(data[i][1] - data[i][2])) maxY = max(data[i][1] + data[i][2], abs(data[i][1] - data[i][2]));
		}
	}
	cout << "Max X:" << maxX << endl;
	cout << "Max Y:" << maxY << endl;
	maxCoord = max(maxX, maxY);
	cout << "MaxCoord:" << maxCoord << endl;
	scalingFactor = (float)maxCoord / (float)(WINDOW_WIDTH / 2);

	cout <<"Scaling factor" << scalingFactor << endl; 



	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			data[i][j] /= scalingFactor;
			printf("%d ", data[i][j]);
		}			
		cout << endl;
	}
}


/*---------------------------------------------------------------------
display(): This function is called once for _every_ frame.
---------------------------------------------------------------------*/
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 0.84, 0);              /* draw in golden yellow */
	glPointSize(1.0);                     /* size of each point */

	glBegin(GL_POINTS);

/*---------------------------------------------------------------------
There are three modes for this program. Option 1 is to draw one single 
circle, the x, y and radius are given by user. Option 2 is to draw se-
veral circle from the given text file. Option 3 is to draw the aniamt-
ion of these circles in option 2.
---------------------------------------------------------------------*/
	if (option == 1) draw_circle(x, y, r);
	if (option == 2) 
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				//Change from OpenGL coordinate System to a World coordinate System
				if (j == 0) x = data[i][j] + WINDOW_WIDTH / 2;
				if (j == 1) y = data[i][j] + WINDOW_WIDTH / 2;
				if (j == 2) r = data[i][j];
			}
			draw_circle(x, y, r);
		}
	}
	if (option == 3)
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (j == 0) x = data[i][j] + WINDOW_WIDTH / 2;
				if (j == 1) y = data[i][j] + WINDOW_WIDTH / 2;
				if (j == 2)	r = data[i][j] * (frame / K);
//				if (r != 0)	cout << r << endl;
			}
			draw_circle(x, y, r);
		}
	}

	glEnd();

	glFlush();                            /* render graphics */

	glutSwapBuffers();                    /* swap buffers */
}

/*---------------------------------------------------------------------
draw_arc
---------------------------------------------------------------------*/
void draw_arc(int x, int y, int r, int xd, int yd)
{
//every time we find a new point to draw, we draw the corresponding the whole 8 points.
	glVertex2i(xd + x, yd + y);
	glVertex2i(-xd + x, yd + y);
	glVertex2i(xd + x, -yd + y);
	glVertex2i(-xd + x, -yd + y);

	glVertex2i(yd + x, xd + y);
	glVertex2i(-yd + x, xd + y);
	glVertex2i(yd + x, -xd + y);
	glVertex2i(-yd + x, -xd + y);
}

/*---------------------------------------------------------------------
draw_circle(x, y, r): the circle center is at (x, y) and the radius is r.
---------------------------------------------------------------------*/
void draw_circle(int x, int y, int r)
{
	int xd = 0;
	int yd = r;

//	float d = 5/4 - r;
	int l = 5 - 4 * r;//l = 4*d. To avoid the floating point.
	while (xd <= yd)
	{
		draw_arc(x, y, r, xd, yd);
		if (l < 0)
		{
			//d += xd * 2 + 3;
			l += xd * 8 + 12;
			xd++;
		}
		else
		{
			//d += 2 * (xd - yd) + 5;
			l += 8 * (xd - yd) + 20;
			yd--;
			xd++;
		}
	}
}

/*---------------------------------------------------------------------
myinit(): Set up attributes and viewing
---------------------------------------------------------------------*/
void myinit()
{
	glClearColor(0.0, 0.0, 0.92, 0.0);    /* blue background*/

	/* set up viewing */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
}

void idle(void)
{
	frame++;
	if (frame > K) frame = 1;
	display();

}