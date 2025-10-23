#define GL_SILENCE_DEPRECATION

#include <GLUT/glut.h>
#include <stdio.h>

// Vertex positions for a single square
float vertices[][3] = {
    {2.0f, 4.0f, 0.0f},
    {8.0f, 4.0f, 0.0f},
    {8.0f, 6.0f, 0.0f},
    {2.0f, 6.0f, 0.0f}
};

// Display callback
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.0f, 1.0f, 0.0f);

    // Draw a square using vertex array (still works in compatibility profile)
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glutSwapBuffers();
}

int main(int argc, char **argv) {
    printf("main() - begin\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Modern Square");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 10, 0, 10, -1, 1);

    glutDisplayFunc(display);
    glutMainLoop();

    printf("main() - end\n");
    return 0;
}
