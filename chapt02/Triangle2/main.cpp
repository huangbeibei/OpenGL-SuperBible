#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
//#include <GLShaderManager.h>
//#include <GLTools.h>

#include <iostream>

//GLBatch triangleBatch;
//GLShaderManager shaderManager;

GLuint hProgram;                // shader����
GLuint vertexArrayObject = 0;   // VAO����
GLuint uiVertexArray = 0;       // VBO����
int vertexAttributeIndex = 0;   // �������������ţ�ֻʹ����0��
int nNumVerts = 3;              // ������Ŀ

/* ---------------------------------
   vertex shader�ᱻÿ��������ã�ͨ��һ����������ܶ���Ϣ���綥�����ꡢ����������������ȣ���Щ����
   ��������ԡ���OpenGL3.0�汾֮ǰ��ÿ���������ض���ͨ�����û�ʹ��glVertex�Ⱥ��������ʺ����ã���shader
   ��ͨ�����ñ���gl_Vertex����������Щ���ԡ���3.0֮����Щ�������������Ƴ��ˡ�������Ҫͨ��ͼ�ζ�������
   ��generic vertex attributes�����ṩ�����ǿ��Ը�����ҪΪÿ�����㶨������������(0��GL_MAX_VERTEX_ATTRIBS-1)
   ���ԣ�����Ϊ��Щ���Զ���һ��������Ȼ��������������ʾͿ����ˡ�
   
   �ڱ����У�����ֻ��һ������vertex shader�е������ζ���λ�����ԣ�����������vertexAttributeIndex��
   �ܽ�һ�¸ó�����shader���õĲ��裺
   1. ����shaderʱ����shader�е�attribute���������ֺ��������Ŷ�Ӧ������
   2. ����һ��VAO���󲢰󶨣�
   3. VAO���ڼ䣬����VBO���������󣬲����������������������֮�����ϵ��
   4. ����ʱ�����VAO����
   5. �����˳�ʱ������VAO��VBO����
*/ 

void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void gltLoadShaderSrc(const char *szShaderSrc, GLuint shader)
{
    GLchar *fsStringPtr[1];

    fsStringPtr[0] = (GLchar *)szShaderSrc;
    glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL);
}

GLuint gltLoadShaderPairSrcWithAttributes(const char *szVertexSrc, const char *szFragmentSrc)
{
    // Temporary Shader objects
    GLuint hVertexShader;
    GLuint hFragmentShader;
    GLuint hReturn = 0;
    GLint testVal;
    
    // Create shader objects
    hVertexShader = glCreateShader(GL_VERTEX_SHADER);
    hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load them
    gltLoadShaderSrc(szVertexSrc, hVertexShader);
    gltLoadShaderSrc(szFragmentSrc, hFragmentShader);

    // Compile them
    glCompileShader(hVertexShader);
    glCompileShader(hFragmentShader);

    // Check for errors
    glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
    if (testVal == GL_FALSE)
    {
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return (GLuint)NULL;
    }
    
    glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
    if (testVal == GL_FALSE)
    {
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return (GLuint)NULL;
    }

    // Link them - assuming it works...
    hReturn = glCreateProgram();
    glAttachShader(hReturn, hVertexShader);
    glAttachShader(hReturn, hFragmentShader);

    // List of attributes(TODO : fixed)
    // Ϊshader�е����Ա���("vVertex")ָ��һ������("vertexAttributeIndex")
    // �������������ԵĶ�Ӧ����������shader���������ǰ����
    glBindAttribLocation(hReturn, vertexAttributeIndex, "vVertex");
    
    glLinkProgram(hReturn);

    // These are no longer needed
    glDeleteShader(hVertexShader);
    glDeleteShader(hFragmentShader);

    // Make sure link worked too
    glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
    if (testVal == GL_FALSE)
    {
        glDeleteProgram(hReturn);
        return (GLuint)NULL;
    }

    return hReturn;
}

static const char *szIdentityShaderVP = "attribute vec4 vVertex;"
                                        "void main(void)"
                                        "{ gl_Position = vVertex;"
                                        "}";

static const char *szIdentityShaderFP = "uniform vec4 vColor;"
                                        "void main(void)"
                                        "{ gl_FragColor = vColor;"
                                        "}";

void SetupRC()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    //shaderManager.InitializeStockShaders();
    hProgram = gltLoadShaderPairSrcWithAttributes(szIdentityShaderVP, szIdentityShaderFP);
    
    GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f };

    // �������󶨶����������VAO
    //triangleBatch.Begin(GL_TRIANGLES, 3);
    vertexArrayObject = 0;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // ��������������VBO����������
    //triangleBatch.CopyVertexData3f(vVerts);
    uiVertexArray = 0;
    glGenBuffers(1, &uiVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3 * nNumVerts, vVerts, GL_DYNAMIC_DRAW);

    //triangleBatch.End();
    // ���������������������������а󶨲��������罨���������Ժͻ������Ĺ�ϵ������������VAO��
    glBindVertexArray(vertexArrayObject);
    if (uiVertexArray != 0)
    {
        // ����OpenGL��ʹ��xxx buffer�ڵ�������Ϊ����Ϊx�Ķ������Ը�ֵ
        glEnableVertexAttribArray(vertexAttributeIndex);
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
        glVertexAttribPointer(vertexAttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    glBindVertexArray(0);
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    GLfloat vRed[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    
    //shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    glUseProgram(hProgram);

    GLint iColor = glGetUniformLocation(hProgram, "vColor");
    glUniform4fv(iColor, 1, vRed);

    //triangleBatch.Draw();
    // ȷ������VAO������ٻ���
    glBindVertexArray(vertexArrayObject);

    glDrawArrays(GL_TRIANGLES, 0, nNumVerts);

    glBindVertexArray(0);

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

    if (uiVertexArray != 0)
        glDeleteBuffers(1, &uiVertexArray);

    glDeleteVertexArrays(1, &vertexArrayObject);

    return 0;
}