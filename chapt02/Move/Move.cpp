// 小动画，演示鼠标操作、程序持续刷新的效果

#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLShaderManager.h>
#include <GLTools.h>

#include <iostream>

GLBatch squareBatch;
GLShaderManager shaderManager;

GLfloat blockSize = 0.1f;
GLfloat vVerts[] = { -blockSize, -blockSize, 0.0f,
                      blockSize, -blockSize, 0.0f,
                      blockSize,  blockSize, 0.0f,
                     -blockSize,  blockSize, 0.0f };

void SetupRC()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    shaderManager.InitializeStockShaders();

    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
}

void SpecialKeys(int key, int x, int y)
{
    GLfloat stepSize = 0.025f;

    GLfloat blockX = vVerts[0];  // Upper left X
    GLfloat blockY = vVerts[7];  // Upper left Y

    if (key == GLUT_KEY_UP)
        blockY += stepSize;

    if (key == GLUT_KEY_DOWN)
        blockY -= stepSize;

    if (key == GLUT_KEY_LEFT)
        blockX -= stepSize;

    if (key == GLUT_KEY_RIGHT)
        blockX += stepSize;

    // Collision detection
    if (blockX < -1.0f) blockX = -1.0f;
    if (blockX >(1.0f - blockSize * 2)) blockX = 1.0f - blockSize * 2;;
    if (blockY < -1.0f + blockSize * 2)  blockY = -1.0f + blockSize * 2;
    if (blockY > 1.0f) blockY = 1.0f;

    // Recalculate vertex positions
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize * 2;

    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY - blockSize * 2;

    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY;

    vVerts[9] = blockX;
    vVerts[10] = blockY;

    squareBatch.CopyVertexData3f(vVerts);

    glutPostRedisplay();
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    squareBatch.Draw();

    glutSwapBuffers();
}

void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Move Block with Arrow Keys");

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);

    SetupRC();

    glutMainLoop();

    return 0;
}