#include <GLUT/glut.h>
#include <stdio.h>

/*
Obtained from:
http://www.dgp.toronto.edu/~ah/csc418/fall_2001/tut/square 
*/
void display(void)  { 
	glClear( GL_COLOR_BUFFER_BIT );
	glColor3f(0, 1, 0);
	glBegin(GL_POLYGON);
		glVertex3f(2, 4, 0);
		glVertex3f(8, 4, 0);
		glVertex3f(8, 6, 0);
		glVertex3f(2, 6, 0);
	glEnd();
	glFlush();
}

int main(int argc, char **argv) 
{ 
	printf("main() - begin\n");
	glutInit(&argc, argv);
	glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(300, 300);
	glutCreateWindow ("square");

	glClearColor(0, 0, 0, 0);     // black background 
	glMatrixMode(GL_PROJECTION);  // setup viewing projection 
	glLoadIdentity();             // start with identity matrix 
	glOrtho(0, 10, 0, 10, -1, 1); // setup a 10x10x2 viewing world

	glutDisplayFunc(  display  );
	glutMainLoop();

	printf("main() - end\n");
	return 0;
}
