#ifndef GAMEFRAME_TEXT_H
#define GAMEFRAME_TEXT_H

#include <cstdio>
#include <glad/glad.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

class Text {
private:
    Program* program;
    Camera* camera;
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint fontTex, vao, vbo;

public:
    explicit Text(Camera* camera) :
        camera(camera),
        program(new Program({new Shader(Shader::Vertex, "../resources/text.vert"),
                             new Shader(Shader::Fragment, "../resources/text.frag")})) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, nullptr, GL_DYNAMIC_DRAW);

            // position/coord attribute
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        program->Upload("uTexture", 0);
    }

    void initFont() {
        auto* ttf_buffer = (unsigned char*)malloc(1 << 20);
        auto* temp_bitmap = (unsigned char*)malloc(512 * 512);

        fread(ttf_buffer, 1, 1 << 20, fopen("C:/Windows/Fonts/times.ttf", "rb"));
        stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
        // can free ttf_buffer at this point
        free(ttf_buffer);
        glGenTextures(1, &fontTex);
        glBindTexture(GL_TEXTURE_2D, fontTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
        // can free temp_bitmap at this point
        free(temp_bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    void display(float x, float y, const glm::vec3& color, const char *text) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        program->Use();
        program->Upload("uColor", color);
        program->Upload("uProjection", camera->getOthro());

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBindTexture(GL_TEXTURE_2D, fontTex);
        Texture::Activate(GL_TEXTURE0);

        stbtt_aligned_quad q;
        while (*text) {
            if (*text >= 32 && *text < 128) {
                stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);
                float verts[] = {
                //  x     y      tX      tY
                    q.x1, q.y0,  q.s1,   q.t0, // tr
                    q.x1, q.y1,  q.s1,   q.t1, // br
                    q.x0, q.y0,  q.s0,   q.t0, // tl
                    q.x1, q.y1,  q.s1,   q.t1, // br
                    q.x0, q.y1,  q.s0,   q.t1, // bl
                    q.x0, q.y0,  q.s0,   q.t0, // tl
                };
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            ++text;
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

};
#endif //GAMEFRAME_TEXT_H
