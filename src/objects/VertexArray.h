#ifndef GAMEFRAME_VERTEXARRAY_H
#define GAMEFRAME_VERTEXARRAY_H

#include <glad/glad.h>

#include "Buffer.h"

class VertexArray {
private:
    static GLuint lastID;
    GLuint id = -1;

public:
    VertexArray() {
        glCreateVertexArrays(1, &this->id);
    }

    void AddSourceBuffer(const Buffer& vbo, int bindingIndex, int stride, int bufferOffset = 0) const {
        glVertexArrayVertexBuffer(this->id, bindingIndex, vbo.getId(), bufferOffset, stride);
    }

    void SetAttribFormat(int index, int numAttribElements, GLenum vertexAttribType, int offset = 0) const {
        glEnableVertexArrayAttrib(this->id, index);
        glVertexArrayAttribFormat(this->id, index, numAttribElements, vertexAttribType, false, offset);
        glVertexArrayAttribBinding(this->id, index, index);
        glVertexArrayBindingDivisor(this->id, index, 0);
    }

    void Bind() const {
        if (this->id == lastID) return;
        glBindVertexArray(this->id);
        lastID = this->id;
    }

    virtual ~VertexArray() {
        glDeleteVertexArrays(1, &this->id);
        this->id = -1;
    }
};

#endif //GAMEFRAME_VERTEXARRAY_H
