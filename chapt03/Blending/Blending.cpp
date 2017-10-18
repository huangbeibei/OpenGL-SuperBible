// GL_BLEND颜色混合的用法，先绘制的物体的颜色如何和后面绘制的物体颜色混合，开启了混合状态下的所有颜色都会混合，包括背景色

#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLShaderManager.h>
#include <GLTools.h>

GLBatch squareBatch;
GLBatch greenBatch;
GLBatch redBatch;
GLBatch blueBatch;
GLBatch blackBatch;

GLShaderManager shaderManager;

GLfloat blockSize = 0.2f;
GLfloat vVerts[] = { -blockSize, -blockSize, 0.0f,
                      blockSize, -blockSize, 0.0f,
                      blockSize,  blockSize, 0.0f,
                     -blockSize,  blockSize, 0.0f };

void SetupRC()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    shaderManager.InitializeStockShaders();

    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();

    GLfloat vBlock[] = { 0.25f, 0.25f, 0.0f,
                         0.75f, 0.25f, 0.0f,
                         0.75f, 0.75f, 0.0f,
                         0.25f, 0.75f, 0.0f};

    greenBatch.Begin(GL_TRIANGLE_FAN, 4);
    greenBatch.CopyVertexData3f(vBlock);
    greenBatch.End();

    GLfloat vBlock2[] = { -0.75f, 0.25f, 0.0f,
                          -0.25f, 0.25f, 0.0f,
                          -0.25f, 0.75f, 0.0f,
                          -0.75f, 0.75f, 0.0f };

    redBatch.Begin(GL_TRIANGLE_FAN, 4);
    redBatch.CopyVertexData3f(vBlock2);
    redBatch.End();

    GLfloat vBlock3[] = { -0.75f, -0.75f, 0.0f,
                          -0.25f, -0.75f, 0.0f,
                          -0.25f, -0.25f, 0.0f,
                          -0.75f, -0.25f, 0.0f };

    blueBatch.Begin(GL_TRIANGLE_FAN, 4);
    blueBatch.CopyVertexData3f(vBlock3);
    blueBatch.End();

    GLfloat vBlock4[] = { 0.25f, -0.75f, 0.0f,
                          0.75f, -0.75f, 0.0f,
                          0.75f, -0.25f, 0.0f,
                          0.25f, -0.25f, 0.0f };

    blackBatch.Begin(GL_TRIANGLE_FAN, 4);
    blackBatch.CopyVertexData3f(vBlock4);
    blackBatch.End();
}

void SpecialKeys(int key, int x, int y)
{
    GLfloat stepSize = 0.025f;

    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[7];

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
    if (blockX > (1.0f - blockSize * 2)) blockX = 1.0f - blockSize * 2;;
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

    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 0.5f };
    GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat vBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vGreen);
    greenBatch.Draw();

    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    redBatch.Draw();

    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlue);
    blueBatch.Draw();

    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlack);
    blackBatch.Draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    squareBatch.Draw();
    glDisable(GL_BLEND);

    // Flush drawing commands
    glutSwapBuffers();
}

void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Move Block with Arrow Keys to see blending");
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);

    SetupRC();
    
    glutMainLoop();
    return 0;
}