#ifndef GAMEFRAME_GLUTILS_H
#define GAMEFRAME_GLUTILS_H

#include <stdio.h>

#include <glad/glad.h>

static int checkGLError(const char* file, int line) {
    const char* message;
    int err;
    switch (err = glGetError()) {
        case GL_NO_ERROR:
            message = "no error";
            break;
        case GL_INVALID_ENUM:
            message = "invalid enum";
            break;
        case GL_INVALID_VALUE:
            message = "invalid name";
            break;
        case GL_INVALID_OPERATION:
            message = "invalid operation";
            break;
        case GL_STACK_OVERFLOW:
            message = "stack overflow";
            break;
        case GL_STACK_UNDERFLOW:
            message = "stack underflow";
            break;
        case GL_OUT_OF_MEMORY:
            message = "out of memory";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            message = "invalid framebuffer";
            break;
        case GL_CONTEXT_LOST:
            message = "context lost";
            break;
        default:
            message = "unknown";
            break;
    }
    printf("GLError (%s, %d): %s\n", file, line, message);
    return err;
}

#define CHECK_GL_ERROR() checkGLError(__FILE__, __LINE__)

#endif //GAMEFRAME_GLUTILS_H
