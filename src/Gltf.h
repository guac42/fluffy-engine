#ifndef FLUFFY_GLTF_H
#define FLUFFY_GLTF_H

#include <iostream>

#include <glad/glad.h>
#include <misc/tiny_gltf.h>
#include "rendering/objects/Buffer.h"
#include "rendering/objects/VertexArray.h"

class Gltf {
private:
    struct Material {
        glm::vec3 diffuse, specular;
    };

    struct Primitive {
        Material* material;
        VertexArray* vao;
    };

    // Mesh stores primitives specific to itself
    struct Mesh {
        std::vector<Primitive> primitives;
    };

    // A Node is a transformed instance of a mesh
    struct Node {
        glm::mat4 transform;
        Mesh* mesh;
    };

    std::vector<Mesh*> meshes;
    std::vector<Texture*> textures;
    std::vector<Material*> materials;

    void loadGltf(const char* filename, tinygltf::Model& model) {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
        if (!warn.empty()) {
            printf("GLTF WARN: %s\n", warn.c_str());
        }

        if (!err.empty()) {
            printf("GLTF ERROR: %s\n", err.c_str());
        }

        if (!res)
            printf("Failed to load glTF: %s\n", filename);
        else
            printf("Loaded glTF: %s\n", filename);
    }

    void loadMaterials(tinygltf::Model& model) {
        materials.reserve(model.materials.size());
        for (const auto &material: model.materials) {
            this->materials.push_back(new Material{
                glm::vec3{material.pbrMetallicRoughness.baseColorFactor[0],
                          material.pbrMetallicRoughness.baseColorFactor[1],
                          material.pbrMetallicRoughness.baseColorFactor[2]
                }
            });
        }
    }

    void bindMesh(tinygltf::Model& model, tinygltf::Mesh& mesh) {
        for (const auto& primitive : mesh.primitives) {
            // Setup element array
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& view = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[view.buffer];
            Primitive prim{materials[primitive.material], new VertexArray()};

            Buffer dataBuffer;
            dataBuffer.Allocate(sizeof(buffer.data.size()), (void*)buffer.data.data(), GL_DYNAMIC_STORAGE_BIT);

            // Setup attribute arrays
            for (const auto &attrib : primitive.attributes) {
                tinygltf::Accessor accessor = model.accessors[attrib.second];

                int size = accessor.type != TINYGLTF_TYPE_SCALAR
                           ? accessor.type : 1;
                int stride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
                int vaa = -1;

                // Attribute type
                if (attrib.first == "POSITION") vaa = 0;
                else if (attrib.first == "NORMAL") vaa = 1;
                else if (attrib.first == "TEXCOORD_0") vaa = 2;

                if (vaa < 0) {
                    printf("Missing Vertex Attribute Array: %s\n", attrib.first.c_str());
                    continue;
                }

                prim.vao->AddSourceBuffer(dataBuffer, vaa, stride, accessor.byteOffset);
                prim.vao->SetAttribFormat(vaa, size, GL_FLOAT);
            }

            // Load texture
            {
                if (model.textures.empty()) continue;

                tinygltf::Texture &tex = model.textures[0];
                if (tex.source < 0) continue;

                tinygltf::Image &image = model.images[tex.source];
                Texture texture(GL_TEXTURE_2D);
                texture.Bind();
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                texture.TextureParam(GL_TEXTURE_WRAP_S, GL_REPEAT);
                texture.TextureParam(GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format;
                GLenum type = GL_UNSIGNED_BYTE;

                switch (image.component) {
                    case 1:
                        format = GL_RED;
                        break;
                    case 2:
                        format = GL_RG;
                        break;
                    case 3:
                        format = GL_RGB;
                        break;
                }

                if (image.bits == 16) type = GL_UNSIGNED_SHORT;

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                             format, type, &image.image.at(0));
            }
        }
    }

    void bindNodes(tinygltf::Model& model, tinygltf::Node& node) {
        if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
            bindMesh(model, model.meshes[node.mesh]);

        for (int i : node.children) {
            assert((i >= 0) && (i < model.nodes.size()));
            bindNodes(model, model.nodes[i]);
        }
    }

    void loadBufferViews(tinygltf::Model& model, std::map<int, GLuint>& vbos) {
        int i = 0;
        for (const auto &view : model.bufferViews) {
            if (!view.target) {// draw array
                printf("glTF Error: Unsupported draw array\n");
                continue;
            }
            const tinygltf::Buffer& buffer = model.buffers[view.buffer];

            GLuint vbo;
            glGenBuffers(1, &vbo);
            vbos[i++] = vbo;

            glBindBuffer(view.target, vbo);
            glBufferData(view.target, view.byteLength,
                         &buffer.data.at(0) + view.byteOffset, GL_STATIC_DRAW);
        }
    }

    /*void renderMesh(tinygltf::Mesh& mesh) {
        for (const auto& primitive : mesh.primitives) {
            tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

            glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
                           &model.buffers[model.bufferViews[indexAccessor.bufferView].buffer].data.at(0) + indexAccessor.byteOffset);
        }
    }

    void renderNodes(tinygltf::Node &node) {
        if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
            renderMesh(model.meshes[node.mesh]);

        for (int i : node.children)
            renderNodes(model.nodes[i]);
    }*/

public:
    explicit Gltf(const char* filename) {
        tinygltf::Model model;
        loadGltf(filename, model);
        loadMaterials(model);

        std::map<int, GLuint> vbos;
        loadBufferViews(model, vbos);

        const tinygltf::Scene &scene = model.scenes[model.defaultScene];
        for (int node : scene.nodes) {
            assert((node >= 0) && (node < model.nodes.size()));
            bindNodes(model, model.nodes[node]);
        }
    }

    void renderModel() {
        /*vao->Bind();

        const tinygltf::Scene &scene = model.scenes[model.defaultScene];
        for (int node : scene.nodes)
            renderNodes(model.nodes[node]);*/
    }

    void modelDebug(tinygltf::Model& model) {
        for (auto &mesh : model.meshes) {
            printf("Mesh: %s\n", mesh.name.c_str());
            for (auto &primitive : mesh.primitives) {
                const tinygltf::Accessor &indexAccessor =
                        model.accessors[primitive.indices];

                printf("\tIndexaccessor: count %zu, type %d\n",
                       indexAccessor.count, indexAccessor.componentType);

                for (auto &mat : model.materials) {
                    printf("\tMat: %s\n", mat.name.c_str());
                    for (const auto &item : mat.values)
                        printf("\t\tParam: %s\n", item.first.c_str());
                }

                std::cout << "\tIndices: " << primitive.indices << std::endl;
                std::cout << "\tMode: "
                          << "(" << primitive.mode << ")" << std::endl;

                for (auto &attrib : primitive.attributes) {
                    std::cout << "\t\tAttribute: " << attrib.first.c_str() << std::endl;
                }
            }
        }
    }
};

#endif //FLUFFY_GLTF_H
