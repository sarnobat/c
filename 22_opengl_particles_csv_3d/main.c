// Modern OpenGL 3D particle viewer (macOS, GLFW + GLEW)
// Usage: ./particles data.csv
// Each line of data.csv: "x,y,z"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct { float x, y, z; } Point;

static Point *points = NULL;
static size_t point_count = 0;

typedef struct {
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
} Bounds;

static Bounds compute_bounds(void) {
    Bounds b;
    if (!point_count) return b;
    b.minX = b.maxX = points[0].x;
    b.minY = b.maxY = points[0].y;
    b.minZ = b.maxZ = points[0].z;
    for (size_t i=1;i<point_count;i++){
        if(points[i].x < b.minX) b.minX = points[i].x;
        if(points[i].x > b.maxX) b.maxX = points[i].x;
        if(points[i].y < b.minY) b.minY = points[i].y;
        if(points[i].y > b.maxY) b.maxY = points[i].y;
        if(points[i].z < b.minZ) b.minZ = points[i].z;
        if(points[i].z > b.maxZ) b.maxZ = points[i].z;
    }
    return b;
}

static void load_csv(const char *path){
    FILE *f = fopen(path,"r");
    if(!f){ perror("open csv"); exit(1);}
    char line[256];
    size_t cap = 0;
    while(fgets(line,sizeof(line),f)){
        float x,y,z;
        if(sscanf(line,"%f,%f,%f",&x,&y,&z)!=3) continue;
        if(point_count+1>cap){
            cap = cap?cap*2:4096;
            points = realloc(points,cap*sizeof(Point));
            if(!points){ perror("realloc"); exit(1);}
        }
        points[point_count++] = (Point){x,y,z};
    }
    fclose(f);
    fprintf(stderr,"Loaded %zu points from %s\n",point_count,path);
}

// Simple 4x4 matrix multiplication
static void multiply4x4(const float a[16], const float b[16], float out[16]){
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            out[j*4+i] = 0.0f;
            for(int k=0;k<4;k++)
                out[j*4+i] += a[k*4+i]*b[j*4+k];
        }
    }
}

// Simple perspective matrix
static void perspective(float fov, float aspect, float near, float far, float out[16]){
    float f = 1.0f / tanf(fov * 0.5f * M_PI / 180.0f);
    for(int i=0;i<16;i++) out[i]=0.0f;
    out[0] = f/aspect;
    out[5] = f;
    out[10] = (far+near)/(near-far);
    out[11] = -1.0f;
    out[14] = (2*far*near)/(near-far);
}

// Simple lookAt matrix
static void lookAt(float eyeX,float eyeY,float eyeZ,
                   float centerX,float centerY,float centerZ,
                   float upX,float upY,float upZ,
                   float out[16]){
    float fx = centerX-eyeX;
    float fy = centerY-eyeY;
    float fz = centerZ-eyeZ;
    float rlen = 1.0f/sqrtf(fx*fx+fy*fy+fz*fz);
    fx*=rlen; fy*=rlen; fz*=rlen;
    float sx = fy*upZ - fz*upY;
    float sy = fz*upX - fx*upZ;
    float sz = fx*upY - fy*upX;
    rlen = 1.0f/sqrtf(sx*sx+sy*sy+sz*sz);
    sx*=rlen; sy*=rlen; sz*=rlen;
    float ux = sy*fz - sz*fy;
    float uy = sz*fx - sx*fz;
    float uz = sx*fy - sy*fx;
    out[0]=sx; out[4]=sy; out[8]=sz;  out[12]=0;
    out[1]=ux; out[5]=uy; out[9]=uz;  out[13]=0;
    out[2]=-fx;out[6]=-fy;out[10]=-fz;out[14]=0;
    out[3]=0; out[7]=0; out[11]=0; out[15]=1;

    // Translate
    out[12] = -(sx*eyeX + sy*eyeY + sz*eyeZ);
    out[13] = -(ux*eyeX + uy*eyeY + uz*eyeZ);
    out[14] = fx*eyeX + fy*eyeY + fz*eyeZ; // note: negative signs handled
}

// Draw bounding box as lines
static void drawBoundingBox(Bounds b){
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    // bottom rectangle
    glVertex3f(b.minX,b.minY,b.minZ); glVertex3f(b.maxX,b.minY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.minZ); glVertex3f(b.maxX,b.minY,b.maxZ);
    glVertex3f(b.maxX,b.minY,b.maxZ); glVertex3f(b.minX,b.minY,b.maxZ);
    glVertex3f(b.minX,b.minY,b.maxZ); glVertex3f(b.minX,b.minY,b.minZ);
    // top rectangle
    glVertex3f(b.minX,b.maxY,b.minZ); glVertex3f(b.maxX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.maxY,b.minZ); glVertex3f(b.maxX,b.maxY,b.maxZ);
    glVertex3f(b.maxX,b.maxY,b.maxZ); glVertex3f(b.minX,b.maxY,b.maxZ);
    glVertex3f(b.minX,b.maxY,b.maxZ); glVertex3f(b.minX,b.maxY,b.minZ);
    // vertical edges
    glVertex3f(b.minX,b.minY,b.minZ); glVertex3f(b.minX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.minZ); glVertex3f(b.maxX,b.maxY,b.minZ);
    glVertex3f(b.maxX,b.minY,b.maxZ); glVertex3f(b.maxX,b.maxY,b.maxZ);
    glVertex3f(b.minX,b.minY,b.maxZ); glVertex3f(b.minX,b.maxY,b.maxZ);
    glEnd();
}

int main(int argc,char**argv){
    if(argc<2){ fprintf(stderr,"Usage: %s data.csv\n",argv[0]); return 1;}
    load_csv(argv[1]);
    Bounds bounds = compute_bounds();

    // Camera setup
    float centerX = (bounds.minX+bounds.maxX)/2.0f;
    float centerY = (bounds.minY+bounds.maxY)/2.0f;
    float centerZ = (bounds.minZ+bounds.maxZ)/2.0f;
    float maxExtent = fmaxf(bounds.maxX-bounds.minX,fmaxf(bounds.maxY-bounds.minY,bounds.maxZ-bounds.minZ));
    float camDist = maxExtent*2.0f;

    if(!glfwInit()) return 1;
    GLFWwindow* win = glfwCreateWindow(800,600,"Particles",NULL,NULL);
    if(!win){ glfwTerminate(); return 1;}
    glfwMakeContextCurrent(win);
    glewInit();
    glEnable(GL_DEPTH_TEST);

    float angle = 0.0f;
    while(!glfwWindowShouldClose(win)){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        int w,h;
        glfwGetFramebufferSize(win,&w,&h);
        float aspect = (float)w/(float)h;

        // Projection
        float proj[16]; perspective(60.0f,aspect,0.1f,1000.0f,proj);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(proj);

        // View
        float camX = centerX + camDist*cosf(angle*M_PI/180.0f);
        float camY = centerY + camDist*0.5f;
        float camZ = centerZ + camDist*sinf(angle*M_PI/180.0f);
        float view[16]; lookAt(camX,camY,camZ, centerX,centerY,centerZ, 0,1,0, view);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(view);

        drawBoundingBox(bounds);

        // Draw points
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        glColor3f(0.7f,0.8f,1.0f);
        for(size_t i=0;i<point_count;i++)
            glVertex3f(points[i].x,points[i].y,points[i].z);
        glEnd();

        glfwSwapBuffers(win);
        glfwPollEvents();
        angle += 0.5f;
    }

    glfwTerminate();
    free(points);
    return 0;
}
