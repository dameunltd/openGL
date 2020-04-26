#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/* validates condition wrapped in parenthesis (insert breakpoint) */
#define ASSERT(x) if (!(x)) __debugbreak(); 

/* macro defined to call GLCearError before function and GLLogCall after 
   ~ (#x) turns function into string to be printed */
#ifdef DEBUG
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)) 
#else
#define GLCall(x) x
#endif // DEBUG


/* clears errors */
static void GLClearError()
{
    /* while loop is equal to (glGetError() != GL_NO_ERROR
       or glGetError does not equal != GL_NO_ERROR */
    while (!glGetError()); 
}

/* prints error messages to console 
    ~ openGL returns error code in an ENUM 
    ~ function: function called
    ~ file: where function was called from
    ~ line: line of code called on */
static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR] ( " << error << ") " << function << " " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    // enum class sets shader mode
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
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
            ss[(int)type] << line << "\n";
        }
    }

    // returns two strings (vertex & type)
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

/* CreateShader function created(static so it doesn't interfere) */
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);


    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "GL Window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* synchronizes refresh rate of the interval swap */
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    /* array created for data(vertex) of triangle (each vertex is the sizeof 2 floats)
        ~ no repetition, all unique data */
    float positions[] = {
          -0.5f, -0.5f,
           0.5f, -0.5f,
           0.5f,  0.5f,
          -0.5f,  0.5f,
    };

    /* tells program to not repeat memory in above function when creating a square
    must be assigned as unsigned int */
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    /* buffer generated and bounded */
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    /* Enable vertex attribute array */
    GLCall(glEnableVertexAttribArray(0));

    /* define an array of generic vertex attribute data
     glVertexAttribPointer(GLuint index, type, normalized(true|false, stride, pointer(const void pointer)

        ~ Stride: # of bytes between each vertex
        ~ Pointer: # of bytes between each attribute */
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    /* index buffer generated and bound (ibo: index buffer object */
    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("res/shading/basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    /* shader bound here */
    GLCall(glUseProgram(shader));

    /* retrieving id/location(name) of Uniform being passed into function 
       ~ glGetUniformLocation(program, name) */
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));

    /* pass through ASSERT if the uniform is being used */
    //ASSERT(location != -1); 

    /* glUniform4f used because vec4 being passed into function 
       ~ vec4 = 4 floats 
       ~ are set per DrawElements call */
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));
    
    float r = 0.0f; // red channel
    float increment = 0.05f; // incrementing animation
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        /* pass in r[red channel] instead of 0.2f like other glUniform() above */
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        /* Draw here
            ~ when using glDrawElements you are drawing the indices, not vertices 
            ~ this error is '1280' but must be translated(see notes in GLCheckError
              function declaration)
            ~ wrapping glDrawElements in GLCall allows for us to execute glClearError
              and GLLogCall function in their respective spots(before & after glDrawElements */
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        
        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;

        /* loop bounces between 1 and 0 for the red channel */
        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}