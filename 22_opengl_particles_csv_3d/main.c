// Modern macOS OpenGL 2.1 particle viewer
// No GLU, uses manual perspective and lookAt matrices
// Usage: ./main data.csv

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
    Bounds b = {0};
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
    // Bottom
    glVertex3f(b.minX,b.minY,b.minZ); glVertex3f(b.maxX,b.minY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.minZ); glVertex3f(b.maxX,b.minY,b.maxZ);
    glVertex3f(b.maxX,b.minY,b.maxZ); glVertex3f(b.minX,b.minY,b.maxZ);
    glVertex3f(b.minX,b.minY,b.maxZ); glVertex3f(b.minX,b.minY,b.minZ);
    // Top
    glVertex3f(b.minX,b.maxY,b.minZ); glVertex3f(b.maxX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.maxY,b.minZ); glVertex3f(b.maxX,b.maxY,b.maxZ);
    glVertex3f(b.maxX,b.maxY,b.maxZ); glVertex3f(b.minX,b.maxY,b.maxZ);
    glVertex3f(b.minX,b.maxY,b.maxZ); glVertex3f(b.minX,b.maxY,b.minZ);
    // Verticals
    glVertex3f(b.minX,b.minY,b.minZ); glVertex3f(b.minX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.minZ); glVertex3f(b.maxX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.maxZ); glVertex3f(b.maxX,b.maxY,b.maxZ);
    glVertex3f(b.minX,b.minY,b.maxZ); glVertex3f(b.minX,b.maxY,b.maxZ);
    glEnd();
}

// Simple perspective matrix (column-major)
static void perspective(float fov_deg, float aspect, float near, float far, float m[16]) {
    float f = 1.0f / tanf(fov_deg * 0.5f * M_PI / 180.0f);
    m[0]  = f/aspect; m[4] = 0; m[8]  = 0;                      m[12] = 0;
    m[1]  = 0;        m[5] = f; m[9]  = 0;                      m[13] = 0;
    m[2]  = 0;        m[6] = 0; m[10] = (far+near)/(near-far); m[14] = (2*far*near)/(near-far);
    m[3]  = 0;        m[7] = 0; m[11] = -1;                     m[15] = 0;
}

// Simple lookAt matrix
static void lookAt(float eyeX,float eyeY,float eyeZ,
                   float centerX,float centerY,float centerZ,
                   float upX,float upY,float upZ,
                   float m[16]) {
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;
    float rlf = 1.0f / sqrtf(fx*fx + fy*fy + fz*fz);
    fx *= rlf; fy *= rlf; fz *= rlf;

    float sx = fy*upZ - fz*upY;
    float sy = fz*upX - fx*upZ;
    float sz = fx*upY - fy*upX;
    float rls = 1.0f / sqrtf(sx*sx + sy*sy + sz*sz);
    sx *= rls; sy *= rls; sz *= rls;

    float ux = sy*fz - sz*fy;
    float uy = sz*fx - sx*fz;
    float uz = sx*fy - sy*fx;

    m[0]=sx; m[4]=sy; m[8]=sz;  m[12]=0;
    m[1]=ux; m[5]=uy; m[9]=uz;  m[13]=0;
    m[2]=-fx;m[6]=-fy;m[10]=-fz;m[14]=0;
    m[3]=0;  m[7]=0;  m[11]=0;  m[15]=1;

    // Translate
    m[12] = -(sx*eyeX + sy*eyeY + sz*eyeZ);
    m[13] = -(ux*eyeX + uy*eyeY + uz*eyeZ);
    m[14] = fx*eyeX + fy*eyeY + fz*eyeZ;
}

int main(int argc, char **argv) {
    if (argc<2){fprintf(stderr,"Usage: %s data.csv\n",argv[0]); return 1;}
    load_csv(argv[1]);

    Bounds bounds = compute_bounds();
    float centerX = (bounds.minX+bounds.maxX)/2.0f;
    float centerY = (bounds.minY+bounds.maxY)/2.0f;
    float centerZ = (bounds.minZ+bounds.maxZ)/2.0f;

    float maxExtent = fmaxf(bounds.maxX-bounds.minX,
                     fmaxf(bounds.maxY-bounds.minY,
                           bounds.maxZ-bounds.minZ));

    float camDist = maxExtent*1.5f;
    float camX = centerX+camDist;
    float camY = centerY+camDist;
    float camZ = centerZ+camDist;

    glfwSetErrorCallback(error_callback);
    if(!glfwInit()) return 1;

    GLFWwindow *win = glfwCreateWindow(800,600,"Particle Viewer",NULL,NULL);
    if(!win){glfwTerminate();return 1;}
    glfwMakeContextCurrent(win);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.0f);
    glClearColor(0.05f,0.05f,0.1f,1.0f);

    float angle=0.0f;
    while(!glfwWindowShouldClose(win)){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // Projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float proj[16];
        perspective(60.0f,800.0f/600.0f,0.1f,1000.0f,proj);
        glLoadMatrixf(proj);

        // ModelView
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        float view[16];
        lookAt(camX,camY,camZ,centerX,centerY,centerZ,0,1,0,view);
        glLoadMatrixf(view);

        glRotatef(angle,0.0f,1.0f,0.0f);
        angle+=0.3f;

        draw_bounding_box(bounds);

        glBegin(GL_POINTS);
        for(size_t i=0;i<point_count;i++){
            glColor3f(0.7f,0.8f,1.0f);
            glVertex3f(points[i].x,points[i].y,points[i].z);
        }
        glEnd();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    free(points);
    return 0;
}
