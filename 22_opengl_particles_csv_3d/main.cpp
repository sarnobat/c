#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

struct Point { float x, y, z; };
struct Bounds { float minX, maxX, minY, maxY, minZ, maxZ; };

static std::vector<Point> points;

static Bounds compute_bounds() {
    Bounds b{};
    if (points.empty()) return b;

    b.minX = b.maxX = points[0].x;
    b.minY = b.maxY = points[0].y;
    b.minZ = b.maxZ = points[0].z;

    for (const auto &p : points) {
        if (p.x < b.minX) b.minX = p.x;
        if (p.x > b.maxX) b.maxX = p.x;
        if (p.y < b.minY) b.minY = p.y;
        if (p.y > b.maxY) b.maxY = p.y;
        if (p.z < b.minZ) b.minZ = p.z;
        if (p.z > b.maxZ) b.maxZ = p.z;
    }
    return b;
}

static void load_csv(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { perror("open csv"); exit(1); }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        float x, y, z;
        if (sscanf(line, "%f,%f,%f", &x, &y, &z) != 3) continue;
        points.push_back({x, y, z});
    }
    fclose(f);
    fprintf(stderr, "Loaded %zu points from %s\n", points.size(), path);
}

static void error_callback(int error, const char *desc) {
    fprintf(stderr, "GLFW error: %s\n", desc);
}

static void draw_bounding_box(const Bounds &b) {
    glColor3f(1.0f, 0.0f, 0.0f); // red
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

// Simple perspective helper
static void perspective(float fovY, float aspect, float near, float far, float out[16]) {
    float f = 1.0f / tanf(fovY * 0.5f * M_PI/180.0f);
    out[0] = f/aspect; out[1]=0; out[2]=0; out[3]=0;
    out[4]=0; out[5]=f; out[6]=0; out[7]=0;
    out[8]=0; out[9]=0; out[10]=(far+near)/(near-far); out[11]=-1;
    out[12]=0; out[13]=0; out[14]=2*far*near/(near-far); out[15]=0;
}

// Simple lookAt helper
static void lookAt(float eyeX,float eyeY,float eyeZ,
                   float centerX,float centerY,float centerZ,
                   float upX,float upY,float upZ,
                   float out[16]) {
    float fx=centerX-eyeX, fy=centerY-eyeY, fz=centerZ-eyeZ;
    float rlen=1.0f/sqrtf(fx*fx+fy*fy+fz*fz); fx*=rlen; fy*=rlen; fz*=rlen;
    float sx=fy*upZ-fz*upY, sy=fz*upX-fx*upZ, sz=fx*upY-fy*upX;
    rlen=1.0f/sqrtf(sx*sx+sy*sy+sz*sz); sx*=rlen; sy*=rlen; sz*=rlen;
    float ux=sy*fz-sz*fy, uy=sz*fx-sx*fz, uz=sx*fy-sy*fx;
    out[0]=sx; out[1]=ux; out[2]=-fx; out[3]=0;
    out[4]=sy; out[5]=uy; out[6]=-fy; out[7]=0;
    out[8]=sz; out[9]=uz; out[10]=-fz; out[11]=0;
    out[12]=-(sx*eyeX+sy*eyeY+sz*eyeZ);
    out[13]=-(ux*eyeX+uy*eyeY+uz*eyeZ);
    out[14]=fx*eyeX+fy*eyeY+fz*eyeZ;
    out[15]=1;
}

int main(int argc,char**argv) {
    if(argc<2){ fprintf(stderr,"Usage: %s data.csv\n",argv[0]); return 1; }
    load_csv(argv[1]);

    Bounds bounds = compute_bounds();
    float centerX=(bounds.minX+bounds.maxX)/2.0f;
    float centerY=(bounds.minY+bounds.maxY)/2.0f;
    float centerZ=(bounds.minZ+bounds.maxZ)/2.0f;
    float maxExtent = std::fmax(bounds.maxX-bounds.minX,
                     std::fmax(bounds.maxY-bounds.minY,
                               bounds.maxZ-bounds.minZ));
    float camX=centerX+maxExtent*1.5f;
    float camY=centerY+maxExtent*1.5f;
    float camZ=centerZ+maxExtent*1.5f;

    glfwSetErrorCallback(error_callback);
    if(!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    GLFWwindow *win = glfwCreateWindow(800,600,"Particles",NULL,NULL);
    if(!win){ glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.0f);
    glClearColor(0.05f,0.05f,0.1f,1.0f);

    float angle=0.0f;
    while(!glfwWindowShouldClose(win)){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        float proj[16]; perspective(60.0f,800.0f/600.0f,0.1f,1000.0f,proj);
        glMatrixMode(GL_PROJECTION); glLoadMatrixf(proj);

        float view[16]; lookAt(camX,camY,camZ,centerX,centerY,centerZ,0,1,0,view);
        glMatrixMode(GL_MODELVIEW); glLoadMatrixf(view);

        glPushMatrix();
        glRotatef(angle,0,1,0);
        draw_bounding_box(bounds);

        glBegin(GL_POINTS);
        for(const auto &p: points){
            glColor3f(0.7f,0.8f,1.0f);
            glVertex3f(p.x,p.y,p.z);
        }
        glEnd();
        glPopMatrix();

        glfwSwapBuffers(win);
        glfwPollEvents();
        angle+=0.3f;
    }

    glfwTerminate();
    return 0;
}
