#ifndef GAMEFRAME_TEXTURE_H
#define GAMEFRAME_TEXTURE_H

#include <glad/glad.h>

class Texture {
public:
    enum Dimension {
        undefined = 0,
        one = 1,
        two = 2,
        three = 3,
    };

    explicit Texture(GLenum target) {
        this->target = target;
        glCreateTextures(target, 1, &this->id);
        switch (target) {
            case GL_TEXTURE_1D:
                this->dimension = one;
                break;
            case GL_TEXTURE_2D:
            case GL_TEXTURE_CUBE_MAP:
                this->dimension = two;
                break;
            case GL_TEXTURE_3D:
                this->dimension = three;
                break;
            default:
                this->dimension = undefined;
                break;
        }
        glTextureParameteri(this->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(this->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    virtual ~Texture() {
        glDeleteTextures(1, &this->id);
    }

    void ImmutableAllocate(int width, int height, int depth, GLenum format) {
        switch (this->dimension) {
            case one:
                glTextureStorage1D(this->id, 1, format, width);
                this->width = width;
                break;
            case two:
                glTextureStorage2D(this->id, 1, format, width, height);
                this->width = width;
                this->height = height;
                break;
            case three:
                glTextureStorage3D(this->id, 1, format, width, height, depth);
                this->width = width;
                this->height = height;
                this->depth = depth;
                break;
            default:
                return;
        }
        this->format = format;
    }

    void Bind() const {
        glBindTexture(this->target, this->id);
    }

    static void Activate(GLenum texture) {
        glActiveTexture(texture);
    }

    void AttachSampler(int unit) const {
        glBindTextureUnit(unit, this->id);
    }

    void TextureParam(GLenum param, GLint value) const {
        glTextureParameteri(this->id, param, value);
    }

    GLuint getId() const {
        return this->id;
    }

private:
    GLuint id = -1;
    GLenum target, format;
    Dimension dimension;
    int width = 0, height = 0, depth = 0;
};

#endif //GAMEFRAME_TEXTURE_H
