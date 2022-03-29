#ifndef GAMEFRAME_BUFFER_H
#define GAMEFRAME_BUFFER_H

#include <glad/glad.h>

class Buffer {
private:
    GLuint id = -1, size = 0;

public:
    Buffer() {
        glCreateBuffers(1, &this->id);
    }

    void Bind(GLenum bufferTarget) const {
        glBindBuffer(bufferTarget, this->id);
    }

    void Allocate(int size, void *data, GLenum bufferStorageFlags) {
        glNamedBufferStorage(this->id, size, data, bufferStorageFlags);
        this->size = size;
    }

    void Data(int size, void *data, GLenum bufferUsage) {
        glNamedBufferData(this->id, size, data, bufferUsage);
        this->size = size;
    }

    GLuint getId() const {
        return this->id;
    }

    unsigned int getSize() const {
        return this->size;
    }

    virtual ~Buffer() {
        glDeleteBuffers(1, &this->id);
        this->id = -1;
    }
};

#endif //GAMEFRAME_BUFFER_H
