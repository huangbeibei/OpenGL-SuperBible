// 剖析Triangle程序最底层源码，理解绘制三角形的过程，体会“OpenGL是一个状态机”

#define FREEGLUT_STATIC
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glut.h>
//#include <GLShaderManager.h>
//#include <GLTools.h>

#include <iostream>

//GLBatch triangleBatch;
//GLShaderManager shaderManager;

GLuint hProgram;                // shader程序
GLuint vertexArrayObject = 0;   // VAO对象（保存了缓冲区对象和顶点属性索引号的关系）
GLuint uiVertexArray = 0;       // VBO对象
int vertexAttributeIndex = 0;   // 顶点属性索引号（只使用了0）
int nNumVerts = 3;              // 顶点数目

/* ---------------------------------
   vertex shader会被每个顶点调用，通常一个顶点包含很多信息，如顶点坐标、法向量、纹理坐标等，这些都是
   顶点的属性。在OpenGL3.0版本之前，每个属性有特定的通道，用户使用glVertex等函数来访问和设置，而shader
   则通过内置变量gl_Vertex等来访问这些属性。但3.0之后，这些都被弃用设置移除了。现在需要通过图形顶点属性
   （generic vertex attributes）来提供。我们可以根据需要为每个顶点定义任意数量的(0到GL_MAX_VERTEX_ATTRIBS-1)
   属性，并且为这些属性定义一个索引，然后根据索引来访问就可以了。
   
   在本例中，属性只有一个，即vertex shader中的三角形顶点位置属性，放置在索引vertexAttributeIndex处
   总结一下该程序中shader设置的步骤：
   1. 创建shader时，将shader中的attribute变量的名字和新索引号对应起来；
   2. 创建一个VAO对象并绑定；
   3. VAO绑定期间，创建VBO缓冲区对象，并建立缓冲区对象和索引号之间的联系；
   4. 绘制时，需绑定VAO对象；
   5. 程序退出时，清理VAO和VBO对象。
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
    // 为shader中的属性变量("vVertex")指定一个索引("vertexAttributeIndex")
    // 这种索引和属性的对应方法必须在shader编译后、链接前调用
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

    // 创建并绑定顶点数组对象VAO
    //triangleBatch.Begin(GL_TRIANGLES, 3);
    vertexArrayObject = 0;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // 建立缓冲区对象VBO，填入数据
    //triangleBatch.CopyVertexData3f(vVerts);
    uiVertexArray = 0;
    glGenBuffers(1, &uiVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 3 * nNumVerts, vVerts, GL_DYNAMIC_DRAW);

    //triangleBatch.End();
    // 这条语句表明，接下来所做的所有绑定操作，例如建立顶点属性和缓冲区的关系，都是针对这个VAO的
    glBindVertexArray(vertexArrayObject);
    if (uiVertexArray != 0)
    {
        // 告诉OpenGL，使用xxx buffer内的数据来为索引为x的顶点属性赋值
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
    // 确保绑定了VAO对象后再绘制
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