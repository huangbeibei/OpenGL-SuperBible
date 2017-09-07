#define FREEGLUT_STATIC

#include <GL/glut.h>

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);

    glutSwapBuffers();

    return 1;
}