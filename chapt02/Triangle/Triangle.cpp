#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLShaderManager.h>
#include <GLTools.h>

#include <iostream>
| 
GLBatch triangleBatch;
GLShaderManager shaderManager;

void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void SetupRC()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    shaderManager.InitializeStockShaders();

    GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f,
                         0.5f, 0.0f, 0.0f,
                         0.0f, 0.5f, 0.0f };

    triangleBatch.Begin(GL_TRIANGLES, 3);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // Question : the order of shader and draw doesn't matter£¿
    GLfloat vRed[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    // input parameter£¬glUseProgram
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
   
    triangleBatch.Draw();
    
    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Triangle");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);

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