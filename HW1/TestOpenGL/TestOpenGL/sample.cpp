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
#define MAXSPEED 400


void display(void);
void myinit(void);
void draw_circle(int x, int y, int r);
void draw_arc(int x, int y, int r, int xd, int yd);
/* Function to handle file input; modification may be needed */
void file_in(void);
void idle(void);
int x, y, r;
int option;	
int rows;
int Speed;
int windowWidth = 0;

int data[MAXLINE][MAXLINE];

/*-----------------
The main function
------------------*/
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

//read from the text file.
	file_in();
/*	errno_t err;
	FILE *stream;
	int maxRadius = 0;//If we read arguements from the txt file, we shall store the max radius.
	int maxXYvalue = 0;//store the Max X value or Y value from the txt file, so that we can accommodate all points.
	char buf[1]; // Store the first row of the circle file.
	int scalingFactor = 3;
	if ((err = fopen_s(&stream, "circles.txt", "r")) != 0)
		printf("The file 'circles.txt' was not opened\n");
	else
		printf("The file 'circles.txt' was opened\n");

	fgets(buf, 10, stream);

	rows = atoi(buf);//Number of circles in the txt file.

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fscanf_s(stream, "%d", &data[i][j]);
			if (j == 0 || j == 1) 
			{
				data[i][j] /= scalingFactor;
				if (maxXYvalue < abs(data[i][j])) maxXYvalue = abs(data[i][j]);
			}
			if (j == 2)
				data[i][j] /= scalingFactor;
				if (maxRadius < data[i][j]) maxRadius = data[i][j];

		}
	}
	windowWidth = (maxRadius + maxXYvalue) * 2;
	printf("%d\n", windowWidth);
	fclose(stream);
	for (int i = 0; i < rows; i++)
	{
		
		for (int j = 0; j < 3; j++)
			printf("%d ", data[i][j]);
		printf("\n");
	}
*/	
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

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	/* Use both double buffering and Z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutIdleFunc(idle);       // Register our idle() function
	glutInitWindowPosition(XOFF, YOFF);
//	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CS6533 Assignment 1");
	glutDisplayFunc(display);



	myinit();
	glutMainLoop();
}

/*----------
file_in(): file input function. Modify here.
------------*/
void file_in(void)
{
	errno_t err;
	FILE *stream;
	int maxRadius = 0;//If we read arguements from the txt file, we shall store the max radius.
	int maxXYvalue = 0;//store the Max X value or Y value from the txt file, so that we can accommodate all points.
	int maxX = 0;
	int maxY = 0;
	int maxCoord;
	char *buf; // Store the first row of the circle file.
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

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fscanf_s(stream, "%d", &data[i][j]);

/*			if (j == 0 || j == 1)
			{
				data[i][j] /= scalingFactor;
				if (maxXYvalue < abs(data[i][j])) maxXYvalue = abs(data[i][j]);
			}
			if (j == 2)
				data[i][j] /= scalingFactor;
			if (maxRadius < data[i][j]) maxRadius = data[i][j];*/

		}
	}
	fclose(stream);

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
	cout << "MaxCoord" << maxCoord << endl;
	scalingFactor = (float)maxCoord / (float)(WINDOW_WIDTH / 2);

	cout <<"What the fuck?" << scalingFactor << endl; 



	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (j == 2) data[i][j] /= scalingFactor;
			printf("%d ", data[i][j]);
		}
			
		printf("\n");
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

	if (option == 1) draw_circle(x, y, r);
	if (option == 2) 
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (j == 0) x = data[i][j] + WINDOW_WIDTH / 2;
				if (j == 1) y = data[i][j] + WINDOW_WIDTH / 2;
				if (j == 2) r = data[i][j] ;

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
				if (j == 2)	r = data[i][j] / Speed;
			}
			draw_circle(x, y, r);
		}
	}



//	glVertex2i(300, 0);               /* draw a vertex here */
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
	int l = 5 - 4 * r;//l = 4*d.
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
	Speed--;  // smaller number gives a slower but smoother animation

	if (Speed <= 1) Speed = MAXSPEED;

	display();

}