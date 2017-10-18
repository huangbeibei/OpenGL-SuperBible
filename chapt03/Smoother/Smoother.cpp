// OpenGL混合功能的另一个用途是抗锯齿。抗锯齿就是把像素的目标颜色与周围像素的颜色进行混合
// 所以要启用抗锯齿GL_POINT_SMOOTH/GL_LINE_SMOOTH，需要同时打开GL_BLEND
// 另外在本机运行时，必须使用独显才能对点进行抗锯齿处理
// 对多边形进行抗锯齿处理一般使用多重采样

#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLTools.h>

GLShaderManager shaderManager;
GLFrustum viewFrustum;
GLBatch smallStarBatch;
GLBatch mediumStarBatch;
GLBatch largeStarBatch;
GLBatch mountainRangeBatch;
GLBatch moonBatch;

#define SMALL_STARS     100
#define MEDIUM_STARS    40
#define LARGE_STARS     15

#define SCREEN_X        800
#define SCREEN_Y        600

void ProcessMenu(int value)
{
    switch (value)
    {
    case 1:
        // Turn on antialiasing, and give hint to do the best job possible.
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        break;

    case 2:
        // Turn off blending and all smoothing
        glDisable(GL_BLEND);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POINT_SMOOTH);
        //glDisable(GL_POLYGON_SMOOTH);
        break;

    default:
        break;
    }
    
    glutPostRedisplay();
}


void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_FLAT, viewFrustum.GetProjectionMatrix(), vWhite);

    // Draw small stars
    glPointSize(1.0f);
    smallStarBatch.Draw();

    // Draw medium sized stars
    glPointSize(4.0f);
    mediumStarBatch.Draw();

    // Draw largest stars
    glPointSize(8.0f);
    largeStarBatch.Draw();

    // Draw the "moon"
    moonBatch.Draw();

    // Draw distant horizon
    glLineWidth(3.5);
    mountainRangeBatch.Draw();

    // Swap buffers
    glutSwapBuffers();
}

void SetupRC()
{
    M3DVector3f vVerts[SMALL_STARS];
    int i;

    shaderManager.InitializeStockShaders();

    // Populate star list
    smallStarBatch.Begin(GL_POINTS, SMALL_STARS);
    for (i = 0; i < SMALL_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % SCREEN_X);
        vVerts[i][1] = (GLfloat)(rand() % (SCREEN_Y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    smallStarBatch.CopyVertexData3f(vVerts);
    smallStarBatch.End();

    // Populate star list
    mediumStarBatch.Begin(GL_POINTS, MEDIUM_STARS);
    for (i = 0; i < MEDIUM_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % SCREEN_X);
        vVerts[i][1] = (GLfloat)(rand() % (SCREEN_Y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    mediumStarBatch.CopyVertexData3f(vVerts);
    mediumStarBatch.End();

    // Populate star list
    largeStarBatch.Begin(GL_POINTS, LARGE_STARS);
    for (i = 0; i < LARGE_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % SCREEN_X);
        vVerts[i][1] = (GLfloat)(rand() % (SCREEN_Y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    largeStarBatch.CopyVertexData3f(vVerts);
    largeStarBatch.End();

    M3DVector3f vMountains[12] = { 0.0f, 25.0f, 0.0f,
                                   50.0f, 100.0f, 0.0f,
                                   100.0f, 25.0f, 0.0f,
                                   225.0f, 125.0f, 0.0f,
                                   300.0f, 50.0f, 0.0f,
                                   375.0f, 100.0f, 0.0f,
                                   460.0f, 25.0f, 0.0f,
                                   525.0f, 100.0f, 0.0f,
                                   600.0f, 20.0f, 0.0f,
                                   675.0f, 70.0f, 0.0f,
                                   750.0f, 25.0f, 0.0f,
                                   800.0f, 90.0f, 0.0f };

    mountainRangeBatch.Begin(GL_LINE_STRIP, 12);
    mountainRangeBatch.CopyVertexData3f(vMountains);
    mountainRangeBatch.End();

    // The Moon
    GLfloat x = 700.0f;
    GLfloat y = 500.0f;
    GLfloat r = 50.0f;
    GLfloat angle = 0.0f;

    moonBatch.Begin(GL_TRIANGLE_FAN, 34);
    int nVerts = 0;
    vVerts[nVerts][0] = x;
    vVerts[nVerts][1] = y;
    vVerts[nVerts][2] = 0.0f;
    for (angle = 0; angle < 2.0f*3.141592f; angle += 0.2f)
    {
        nVerts++;
        vVerts[nVerts][0] = x + float(cos(angle)) * r;
        vVerts[nVerts][1] = y + float(sin(angle)) * r;
        vVerts[nVerts][2] = 0.0f;
    }
    nVerts++;
    
    vVerts[nVerts][0] = x + r;
    vVerts[nVerts][1] = y;
    vVerts[nVerts][2] = 0.0f;
    moonBatch.CopyVertexData3f(vVerts);
    moonBatch.End();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void ChangeSize(int w, int h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    viewFrustum.SetOrthographic(0.0f, SCREEN_X, 0.0f, SCREEN_Y, -1.0f, 1.0f);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Smoothing Out The Jaggies");

    // Create the Menu
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Antialiased Rendering", 1);
    glutAddMenuEntry("Normal Rendering", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

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