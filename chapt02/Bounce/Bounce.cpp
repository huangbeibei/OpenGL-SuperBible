// 小动画，程序自动刷新的效果

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
GLfloat vVerts[] = { -blockSize - 0.5f, -blockSize, 0.0f,
                      blockSize - 0.5f, -blockSize, 0.0f,
                      blockSize - 0.5f,  blockSize, 0.0f,
                     -blockSize - 0.5f,  blockSize, 0.0f};

void SetupRC()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    shaderManager.InitializeStockShaders();

    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
}

void BounceFunction(void)
{
    static GLfloat xDir = 1.0f;
    static GLfloat yDir = 1.0f;

    GLfloat stepSize = 0.005f;

    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[7];

    blockY += stepSize * yDir;
    blockX += stepSize * xDir;

    if (blockX < -1.0f) { blockX = -1.0f; xDir *= -1.0f; }
    if (blockX >(1.0f - blockSize * 2)) { blockX = 1.0f - blockSize * 2; xDir *= -1.0f; }
    if (blockY < -1.0f + blockSize * 2)  { blockY = -1.0f + blockSize * 2; yDir *= -1.0f; }
    if (blockY > 1.0f) { blockY = 1.0f; yDir *= -1.0f; }

    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize * 2;

    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY - blockSize * 2;

    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY;

    vVerts[9] = blockX;
    vVerts[10] = blockY;

    squareBatch.CopyVertexData3f(vVerts);
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    squareBatch.Draw();

    glutSwapBuffers();

    // RenderScene is called when window minimizing/window maximizing/window be covered/window display again, 
    // it's not called for every frame. 
    // When the scene should be rendered, glutPostRedisplay is called
    BounceFunction();
    glutPostRedisplay();
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
    glutCreateWindow("Bouncing Block");

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

    SetupRC();

    glutMainLoop();

    return 0;
}