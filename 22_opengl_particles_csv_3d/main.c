// Simple OpenGL 3D particle viewer
// Usage: ./particles data.csv
// Each line of data.csv: "x,y,z"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct {
    float x, y, z;
} Point;

static Point *points = NULL;
static size_t point_count = 0;

static void load_csv(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { perror("open csv"); exit(1); }

    char line[256];
    size_t cap = 0;
    while (fgets(line, sizeof(line), f)) {
        float x, y, z;
        if (sscanf(line, "%f,%f,%f", &x, &y, &z) != 3) continue;
        if (point_count + 1 > cap) {
            cap = cap ? cap * 2 : 4096;
            points = realloc(points, cap * sizeof(Point));
            if (!points) { perror("realloc"); exit(1); }
        }
        points[point_count++] = (Point){x, y, z};
    }
    fclose(f);
    fprintf(stderr, "Loaded %zu points from %s\n", point_count, path);
}

static void error_callback(int error, const char *desc) {
    fprintf(stderr, "GLFW error: %s\n", desc);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s data.csv\n", argv[0]);
        return 1;
    }
    load_csv(argv[1]);

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow *win = glfwCreateWindow(800, 600, "Particle Viewer", NULL, NULL);
    if (!win) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(win);
    glewInit();

    glEnable(GL_POINT_SMOOTH);
    glPointSize(3.0f);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    float angle = 0.0f;
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, 800.0/600.0, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -3.0f);
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        angle += 0.3f;

        glBegin(GL_POINTS);
        for (size_t i = 0; i < point_count; i++) {
            glColor3f(0.7f, 0.8f, 1.0f);
            glVertex3f(points[i].x, points[i].y, points[i].z);
        }
        glEnd();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    free(points);
    return 0;
}
