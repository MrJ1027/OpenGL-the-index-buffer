#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include <fstream>
#include<string>
#include <sstream>
//处理错误
static void GLClearError()
{
    while (glGetError != GL_NO_ERROR);
}

static void GLCheckError()
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

//设置实际的着色器源码
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    std::string line;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id );

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);//获取着色器对象的信息，这里是编译状态（是否编译成功）
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);//获取该着色器对象的日志字符串你长度，length参数会被设置为缓存区中实际写入的字符数量
        char* message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static unsigned int CreatShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs= CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(900, 700, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
      glewInit();//必须要加这个初始化
    float positions[12] = {
        -0.5f,-0.5f,//0
         0.5f, -0.5f,//1
         0.5f, 0.5f,//2
         -0.5f,0.5f,//3
    };
    unsigned int indices[] =
    {
        0,1,2,
        2,3,0
    };

    unsigned int buffer = 0;
    glGenBuffers(1, &buffer);//creat buffer
    glBindBuffer(GL_ARRAY_BUFFER,buffer);//绑定buffer
    glBufferData(GL_ARRAY_BUFFER, 6 *2* sizeof(float), positions, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    unsigned int ibo = 0;
    glGenBuffers(1, &ibo);//creat ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);//绑定ibo
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 *sizeof(unsigned int),indices, GL_STREAM_DRAW);

 
    ShaderProgramSource source = ParseShader("res/Shaders/Basic.shader");
    //测试：
    /*  std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource<< std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;*/


    unsigned int shader = CreatShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
       // glDrawArrays(GL_TRIANGLES, 0, 6);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
   glDeleteProgram(shader);//删除着色器程序对象

    glfwTerminate();
    return 0;
}