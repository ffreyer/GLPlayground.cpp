#pragma once

#include <vector>

#include "../opengl/GLTexture.hpp"
#include "../opengl/GLTexture.hpp"
#include "../opengl/GLVertexArray.hpp"
#include "../opengl/GLShader.hpp"

#include "../Scene/Entity.hpp"

namespace Component {
    struct SimpleMesh {
        std::vector<uint32_t> indices;
        std::vector<float> vertices;
        GLVertexArray va;

        // TODO: check if move is the correct tool here
        SimpleMesh(std::vector<uint32_t>& idxs, std::vector<float>& data, GLBufferLayout& layout);

        void bind() const { va.bind(); }
        void unbind() const { va.unbind(); }
        uint32_t size() const { return va.index_count(); }
    };

    struct SimpleTexture2D {
        int width = 0, height = 0;
        GLTexture texture;

        SimpleTexture2D(const char* filepath);

        void bind(GLShader& shader, unsigned int slot) const;
        void bind() const { texture.bind(); }
        void unbind() const { texture.unbind(); }
    };
}

// TODO: make a parent class for this
class MeshRenderer {
private:
    std::shared_ptr<GLShader> m_shader = nullptr;

public:
    MeshRenderer() = default;
    void init();

    // add cube components to existing entity
    void add_cube_mesh(Entity& cube) const;

    // rendering
    void begin(glm::mat4& projectionview) const;
    void draw_mesh(Entity e) const;
    void end() const;
};