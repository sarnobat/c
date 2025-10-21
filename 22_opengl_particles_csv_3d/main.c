// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define MAX_LINE 1024

typedef struct {
    float x,y,z;
    float r,g,b;
} Pnt;

static Pnt *points = NULL;
static size_t point_count = 0;

void load_csv(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { perror("open csv"); exit(1); }
    char line[MAX_LINE];
    size_t cap = 0;
    while (fgets(line, sizeof(line), f)) {
        char *s = line;
        while (*s && (*s == ' ' || *s == '\t')) s++;
        if (*s == '\0' || *s == '\n' || *s == '#') continue;
        float vals[6] = {0,0,0, 1.0f,1.0f,1.0f};
        int n = 0;
        // allow comma or whitespace separated
        char *tok = strtok(line, ", \t\r\n");
        while (tok && n < 6) {
            vals[n++] = atof(tok);
            tok = strtok(NULL, ", \t\r\n");
        }
        if (point_count+1 > cap) {
            cap = cap ? cap*2 : 4096;
            points = realloc(points, cap * sizeof(Pnt));
        }
        points[point_count].x = vals[0];
        points[point_count].y = vals[1];
        points[point_count].z = vals[2];
        if (n >= 6) {
            points[point_count].r = vals[3] / 255.0f;
            points[point_count].g = vals[4] / 255.0f;
            points[point_count].b = vals[5] / 255.0f;
        } else {
            points[point_count].r = vals[3];
            points[point_count].g = vals[4];
            points[point_count].b = vals[5];
        }
        point_count++;
    }
    fclose(f);
    fprintf(stderr, "Loaded %zu points\n", point_count);
}

// simple shader creation helpers
GLuint compile_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[10240]; glGetShaderInfoLog(s, sizeof(buf), NULL, buf);
        fprintf(stderr, "Shader compile failed: %s\n", buf);
        exit(1);
    }
    return s;
}

const char *vert_src =
"#version 330 core\n"
"layout(location=0) in vec3 in_pos;\n"
"layout(location=1) in vec3 in_col;\n" 
"out vec3 vcol;\n"
"uniform float u_time;\n"
"void main() {\n"
"    // tiny per-vertex wobble using position-based phase\n"
"    float phase = dot(in_pos, vec3(12.9898,78.233,37.719));\n"
"    float offs = 0.01 * sin(u_time * 3.0 + phase);\n"
"    vec3 p = in_pos + normalize(in_pos) * offs;\n" 
"    gl_Position = vec4(p, 1.0);\n"
"    vcol = in_col;\n" 
"    gl_PointSize = 3.0;\n"
"}\n";

const char *frag_src =
"#version 330 core\n"
"in vec3 vcol;\n" 
"out vec4 out_col;\n"
"void main() {\n"
"    float d = length(gl_PointCoord - vec2(0.5));\n"    // circular points
"    if (d > 0.5) discard;\n"
"    out_col = vec4(vcol, 1.0);\n"
"}\n";

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s data.csv\n", argv[0]); return 2; }
    load_csv(argv[1]);

    if (!glfwInit()) { fprintf(stderr, "glfwInit failed\n"); return 1; }
    // Request core 3.3 (on macOS you might need 3.2)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(800, 600, "Particles", NULL, NULL);
    if (!win) { fprintf(stderr, "glfwCreateWindow failed\n"); glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { fprintf(stderr, "glew init failed\n"); return 1; }
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // build shader
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vert_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, frag_src);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint linked; glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) { char buf[10240]; glGetProgramInfoLog(prog, sizeof(buf), NULL, buf); fprintf(stderr, "Link failed: %s\n", buf); return 1; }
    glUseProgram(prog);

    // upload VBO
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // interleave pos(3) + col(3)
    size_t stride = sizeof(float) * 6;
    float *buf = malloc(point_count * 6 * sizeof(float));
    for (size_t i=0;i<point_count;i++) {
        buf[i*6+0] = points[i].x;
        buf[i*6+1] = points[i].y;
        buf[i*6+2] = points[i].z;
        buf[i*6+3] = points[i].r;
        buf[i*6+4] = points[i].g;
        buf[i*6+5] = points[i].b;
    }
    glBufferData(GL_ARRAY_BUFFER, point_count * stride, buf, GL_STATIC_DRAW);
    free(buf);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);

    GLint uni_time = glGetUniformLocation(prog, "u_time");

    // very simple view/projection: scale into clip-space. You may want a proper MVP.
    // Find bounding box to normalize coordinates
    float minx=1e9,miny=1e9,minz=1e9,maxx=-1e9,maxy=-1e9,maxz=-1e9;
    for (size_t i=0;i<point_count;i++){
        minx = fmin(minx, points[i].x);
        miny = fmin(miny, points[i].y);
        minz = fmin(minz, points[i].z);
        maxx = fmax(maxx, points[i].x);
        maxy = fmax(maxy, points[i].y);
        maxz = fmax(maxz, points[i].z);
    }
    float cx = (minx+maxx)/2.0f;
    float cy = (miny+maxy)/2.0f;
    float cz = (minz+maxz)/2.0f;
    float r = fmax(fmax(maxx-minx, maxy-miny), maxz-minz);
    // normalize CPU-side to clip space [-0.9,0.9]
    // remap VBO positions in place for convenience
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float *mapped = (float*)malloc(point_count * 6 * sizeof(float));
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, point_count * 6 * sizeof(float), mapped);
    for (size_t i=0;i<point_count;i++){
        mapped[i*6+0] = (mapped[i*6+0] - cx) / r * 1.6f;
        mapped[i*6+1] = (mapped[i*6+1] - cy) / r * 1.6f;
        mapped[i*6+2] = (mapped[i*6+2] - cz) / r * 1.6f;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, point_count * 6 * sizeof(float), mapped);
    free(mapped);

    double t0 = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {
        double t = glfwGetTime() - t0;
        int w,h; glfwGetFramebufferSize(win,&w,&h);
        glViewport(0,0,w,h);
        glClearColor(0.05f,0.05f,0.07f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prog);
        glUniform1f(uni_time, (float)t);

        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, (GLsizei)point_count);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    free(points);
    return 0;
}
