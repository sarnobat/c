// Simple OpenGL 3D particle viewer with visible bounding box
// Usage: ./particles data.csv
// Each line of data.csv: "x,y,z"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct {
    float x, y, z;
} Point;

static Point *points = NULL;
static size_t point_count = 0;

typedef struct {
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
} Bounds;

static Bounds compute_bounds(void) {
    Bounds b;
    if (point_count == 0) return b;

    b.minX = b.maxX = points[0].x;
    b.minY = b.maxY = points[0].y;
    b.minZ = b.maxZ = points[0].z;

    for (size_t i = 1; i < point_count; i++) {
        if (points[i].x < b.minX) b.minX = points[i].x;
        if (points[i].x > b.maxX) b.maxX = points[i].x;
        if (points[i].y < b.minY) b.minY = points[i].y;
        if (points[i].y > b.maxY) b.maxY = points[i].y;
        if (points[i].z < b.minZ) b.minZ = points[i].z;
        if (points[i].z > b.maxZ) b.maxZ = points[i].z;
    }
    return b;
}

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

// Draw red bounding box edges
static void draw_bounding_box(Bounds b) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // Bottom rectangle
    glVertex3f(b.minX,b.minY,b.minZ); glVertex3f(b.maxX,b.minY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.minZ); glVertex3f(b.maxX,b.minY,b.maxZ);
    glVertex3f(b.maxX,b.minY,b.maxZ); glVertex3f(b.minX,b.minY,b.maxZ);
    glVertex3f(b.minX,b.minY,b.maxZ); glVertex3f(b.minX,b.minY,b.minZ);
    // Top rectangle
    glVertex3f(b.minX,b.maxY,b.minZ); glVertex3f(b.maxX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.maxY,b.minZ); glVertex3f(b.maxX,b.maxY,b.maxZ);
    glVertex3f(b.maxX,b.maxY,b.maxZ); glVertex3f(b.minX,b.maxY,b.maxZ);
    glVertex3f(b.minX,b.maxY,b.maxZ); glVertex3f(b.minX,b.maxY,b.minZ);
    // Vertical edges
    glVertex3f(b.minX,b.minY,b.minZ); glVertex3f(b.minX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.minZ); glVertex3f(b.maxX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.maxZ); glVertex3f(b.maxX,b.maxY,b.maxZ);
    glVertex3f(b.minX,b.minY,b.maxZ); glVertex3f(b.minX,b.maxY,b.maxZ);
    glEnd();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s data.csv\n", argv[0]);
        return 1;
    }
    load_csv(argv[1]);

    Bounds bounds = compute_bounds();
    float centerX = (bounds.minX + bounds.maxX) / 2.0f;
    float centerY = (bounds.minY + bounds.maxY) / 2.0f;
    float centerZ = (bounds.minZ + bounds.maxZ) / 2.0f;

    float maxExtent = fmaxf(bounds.maxX - bounds.minX,
                     fmaxf(bounds.maxY - bounds.minY,
                           bounds.maxZ - bounds.minZ));

    // Camera offset: move back 2× maxExtent diagonally
    float camDistance = maxExtent * 2.0f;
    float camX = centerX + camDistance;
    float camY = centerY + camDistance;
    float camZ = centerZ + camDistance;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow *win = glfwCreateWindow(800, 600, "Particle Viewer", NULL, NULL);
    if (!win) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(win);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.0f); // larger points
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    float angle = 0.0f;
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, 800.0/600.0, 0.1, 1000.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camX, camY, camZ,    // camera position
                  centerX, centerY, centerZ,  // look at center
                  0.0f, 1.0f, 0.0f);  // up vector

        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        angle += 0.3f;

        // Draw bounding box
        draw_bounding_box(bounds);

        // Draw points
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
