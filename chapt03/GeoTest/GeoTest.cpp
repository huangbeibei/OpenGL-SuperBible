// 前后裁剪面GL_CULL_FACE、深度测试DEPTH_TEST:OpenGL的内置功能保证绘制效果不会因为物体绘制的先后顺序出现问题

#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLShaderManager.h>
#include <GLTools.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>

GLFrame             viewFrame;
GLFrustum           viewFrustum;
GLShaderManager     shaderManager;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;

// Flags for effects
int iCull = 0;
int iDepth = 0;

void ProcessMenu(int value)
{
    switch (value)
    {
    case 1:
        iDepth = !iDepth;
        break;

    case 2:
        iCull = !iCull;
        break;

    case 3:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case 4:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;

    case 5:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    }

    glutPostRedisplay();
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (iCull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    if (iDepth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    modelViewMatrix.PushMatrix(viewFrame);

    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);

    torusBatch.Draw();

    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

void SetupRC()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    shaderManager.InitializeStockShaders();
    viewFrame.MoveForward(7.0f);

    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);

    glPointSize(4.0f);
}

void SpecialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    
    if (key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);

    if (key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);

    if (key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);

    glutPostRedisplay();
}

void ChangeSize(int w, int h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 100.0f);

    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Geometry Test Program");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);

    // Create the Menu
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Toggle depth test", 1);
    glutAddMenuEntry("Toggle cull backface", 2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    SetupRC();
    
    glutMainLoop();
    return 0;
}