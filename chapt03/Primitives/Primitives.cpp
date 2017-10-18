// OpenGL不同类型的几何图元、为了避免z-fighting的glPolygonOffset设置、多边形的点线绘制模式

#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLShaderManager.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrame.h>
#include <GLBatch.h>
#include <GLFrustum.h>
#include <iostream>


GLShaderManager     shaderManager;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLFrame             cameraFrame;
GLFrame             objectFrame;
GLFrustum           viewFrustum;

GLBatch             pointBatch;
GLBatch             lineBatch;
GLBatch             lineStripBatch;
GLBatch             lineLoopBatch;
GLBatch             triangleBatch;
GLBatch             triangleStripBatch;
GLBatch             triangleFanBatch;

GLGeometryTransform transformPipeline;

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

int nStep = 0;

 
void SetupRC()
{
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

    shaderManager.InitializeStockShaders();

    glEnable(GL_DEPTH_TEST);

    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

    cameraFrame.MoveForward(-15.0f);

    //////////////////////////////////////////////////////////////////////
    // Some points, more or less in the shape of Florida
    GLfloat vCoast[24][3] = { { 2.80, 1.20, 0.0 }, { 2.0, 1.20, 0.0 },
                            { 2.0, 1.08, 0.0 }, { 2.0, 1.08, 0.0 },
                            { 0.0, 0.80, 0.0 }, { -.32, 0.40, 0.0 },
                            { -.48, 0.2, 0.0 }, { -.40, 0.0, 0.0 },
                            { -.60, -.40, 0.0 }, { -.80, -.80, 0.0 },
                            { -.80, -1.4, 0.0 }, { -.40, -1.60, 0.0 },
                            { 0.0, -1.20, 0.0 }, { .2, -.80, 0.0 },
                            { .48, -.40, 0.0 }, { .52, -.20, 0.0 },
                            { .48, .20, 0.0 }, { .80, .40, 0.0 },
                            { 1.20, .80, 0.0 }, { 1.60, .60, 0.0 },
                            { 2.0, .60, 0.0 }, { 2.2, .80, 0.0 },
                            { 2.40, 1.0, 0.0 }, { 2.80, 1.0, 0.0 } };

    // Load point batch
    pointBatch.Begin(GL_POINTS, 24);
    pointBatch.CopyVertexData3f(vCoast);
    pointBatch.End();

    // Load as a bunch of line segments
    lineBatch.Begin(GL_LINES, 24);
    lineBatch.CopyVertexData3f(vCoast);
    lineBatch.End();

    // Load as a single line segment
    lineStripBatch.Begin(GL_LINE_STRIP, 24);
    lineStripBatch.CopyVertexData3f(vCoast);
    lineStripBatch.End();

    // Single line, connect first and last points
    lineLoopBatch.Begin(GL_LINE_LOOP, 24);
    lineLoopBatch.CopyVertexData3f(vCoast);
    lineLoopBatch.End();

    // For Triangles, we'll make a Pyramid
    GLfloat vPyramid[12][3] = { -2.0f, 0.0f, -2.0f,
                                2.0f, 0.0f, -2.0f,
                                0.0f, 4.0f, 0.0f,

                                2.0f, 0.0f, -2.0f,
                                2.0f, 0.0f, 2.0f,
                                0.0f, 4.0f, 0.0f,

                                2.0f, 0.0f, 2.0f,
                                -2.0f, 0.0f, 2.0f,
                                0.0f, 4.0f, 0.0f,

                                -2.0f, 0.0f, 2.0f,
                                -2.0f, 0.0f, -2.0f,
                                0.0f, 4.0f, 0.0f };

    triangleBatch.Begin(GL_TRIANGLES, 12);
    triangleBatch.CopyVertexData3f(vPyramid);
    triangleBatch.End();

    GLfloat vStripPoints[100][3];    // Scratch array, more than we need
    // For triangle strips, a little ring or cylinder segment
    int iCounter = 0;
    GLfloat radius = 3.0f;
    for (GLfloat angle = 0.0f; angle <= (2.0f*M3D_PI); angle += 0.3f)
    {
        GLfloat x = radius * sin(angle);
        GLfloat y = radius * cos(angle);

        // Specify the point and move the Z value up a little
        vStripPoints[iCounter][0] = x;
        vStripPoints[iCounter][1] = y;
        vStripPoints[iCounter][2] = -0.5;
        iCounter++;

        vStripPoints[iCounter][0] = x;
        vStripPoints[iCounter][1] = y;
        vStripPoints[iCounter][2] = 0.5;
        iCounter++;
    }

    // Close up the loop
    vStripPoints[iCounter][0] = vStripPoints[0][0];
    vStripPoints[iCounter][1] = vStripPoints[0][1];
    vStripPoints[iCounter][2] = -0.5;
    iCounter++;

    vStripPoints[iCounter][0] = vStripPoints[1][0];
    vStripPoints[iCounter][1] = vStripPoints[1][1];
    vStripPoints[iCounter][2] = 0.5;
    iCounter++;

    // Load the triangle strip
    triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    triangleStripBatch.CopyVertexData3f(vStripPoints);
    triangleStripBatch.End();

    // For a Triangle fan, just a 6 sided hex. Raise the center up a bit
    GLfloat vFanPoints[100][3];

    int nVerts = 0;
    GLfloat r = 3.0f;
    vFanPoints[nVerts][0] = 0.0f;
    vFanPoints[nVerts][1] = 0.0f;
    vFanPoints[nVerts][2] = 0.0f;

    for (GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI / 6.0f)
    {
        nVerts++;
        vFanPoints[nVerts][0] = float(cos(angle)) * r;
        vFanPoints[nVerts][1] = float(sin(angle)) * r;
        vFanPoints[nVerts][2] = -0.5f;
    }

    // Close the fan
    nVerts++;
    vFanPoints[nVerts][0] = r;
    vFanPoints[nVerts][1] = 0;
    vFanPoints[nVerts][2] = 0.0f;

    // Load it up
    triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
    triangleFanBatch.CopyVertexData3f(vFanPoints);
    triangleFanBatch.End();
}

void DrawWireFramedBatch(GLBatch* pBatch)
{
    // Draw the batch solid green
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();

    // Draw black outline
    glPolygonOffset(-1.0f, -1.0f);      // Shift depth values
    glEnable(GL_POLYGON_OFFSET_LINE);

    // Draw lines antialiased
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw black wireframe version of geometry
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.5f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();

    // Put everything back the way we found it
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);

    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);

    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);

    switch (nStep)
    {
    case 0:
        glPointSize(4.0f);
        pointBatch.Draw();
        glPointSize(1.0f);
        break;
    case 1:
        glLineWidth(2.0f);
        lineBatch.Draw();
        glLineWidth(1.0f);
        break;
    case 2:
        glLineWidth(2.0f);
        lineStripBatch.Draw();
        glLineWidth(1.0f);
        break;
    case 3:
        glLineWidth(2.0f);
        lineLoopBatch.Draw();
        glLineWidth(1.0f);
        break;
    case 4:
        DrawWireFramedBatch(&triangleBatch);
        break;
    case 5:
        DrawWireFramedBatch(&triangleStripBatch);
        break;
    case 6:
        DrawWireFramedBatch(&triangleFanBatch);
        break;
    }

    modelViewMatrix.PopMatrix();

    // Flush drawing commands
    glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);

    if (key == GLUT_KEY_DOWN)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);

    if (key == GLUT_KEY_LEFT)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);

    if (key == GLUT_KEY_RIGHT)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);

    glutPostRedisplay();
}

void KeyPressFunc(unsigned char key, int x, int y)
{
    if (key == 32)
    {
        nStep++;

        if (nStep > 6)
            nStep = 0;
    }

    switch (nStep)
    {
    case 0:
        glutSetWindowTitle("GL_POINTS");
        break;
    case 1:
        glutSetWindowTitle("GL_LINES");
        break;
    case 2:
        glutSetWindowTitle("GL_LINE_STRIP");
        break;
    case 3:
        glutSetWindowTitle("GL_LINE_LOOP");
        break;
    case 4:
        glutSetWindowTitle("GL_TRIANGLES");
        break;
    case 5:
        glutSetWindowTitle("GL_TRIANGLE_STRIP");
        break;
    case 6:
        glutSetWindowTitle("GL_TRIANGLE_FAN");
        break;
    }

    glutPostRedisplay();
}

void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelViewMatrix.LoadIdentity();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("GL_POINTS");
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(KeyPressFunc);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "GLEW Error : %s\n", glewGetErrorString(err));
        return 1;
    }

    SetupRC();

    glutMainLoop();

    return 0;
}