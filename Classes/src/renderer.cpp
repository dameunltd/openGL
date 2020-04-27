#include "renderer.h"

#include <iostream>

void GLClearError()
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
bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR] ( " << error << ") " << function << " " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}