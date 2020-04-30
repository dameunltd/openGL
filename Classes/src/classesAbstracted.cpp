#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "renderer.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "VertexArray.h"
#include "shader.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    /* scope created from here to before glTerminate function to make sure program
       terminates */
    {
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

        /* vertex array object(vao) generated and bounded
           ~ this binds the vertex buffer and it's layout */
        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        /* vertex buffer generated and bounded thru VertexBuffer class in vertexbuffer.h */
        VertexBuffer vb(positions, 6 * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.addBuffer(vb, layout);

        /* index buffer generated and bound (ibo: index buffer object */
        IndexBuffer ib(indices, 6);

        Shader shader("res/shading/basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        /* buffers and shader unbound here */
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        float r = 0.0f; // red channel
        float increment = 0.05f; // incrementing animation
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            shader.Bind();
            /* pass in r[red channel] instead of 0.2f like other glUniform() above */
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            va.Bind();
            /* index buffer bind from indexbuffer.h/indexbuffer.cpp file */
            ib.Bind();

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
    }

    glfwTerminate();
    return 0;
}