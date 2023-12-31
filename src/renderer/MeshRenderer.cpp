#include "MeshRenderer.hpp"

namespace Component {
    // TODO: check if move is the correct tool here
    SimpleMesh::SimpleMesh(std::vector<uint32_t>& idxs, std::vector<float>& data, GLBufferLayout& layout)
        : indices(idxs), vertices(data)
    {
        std::shared_ptr<GLIndexBuffer> ibuffer = std::make_shared<GLIndexBuffer>(indices);
        std::shared_ptr<GLVertexBuffer> vbuffer = std::make_shared<GLVertexBuffer>(vertices);
        vbuffer->set_layout(layout);
        va.set(ibuffer);
        va.push(vbuffer);
    }

    SimpleTexture2D::SimpleTexture2D(const char* filepath) 
        : texture(GLTexture(2))
    {
        int channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* image = stbi_load(filepath, &width, &height, &channels, 0);
        unsigned int format = channels_to_gl_type(channels);
        texture.set_internal_format(format);
        texture.set_data(image, format, width, height);
        stbi_image_free(image);
    };
}

void MeshRenderer::init() {
    m_shader = std::make_shared<GLShader>();
    m_shader->add_source("../assets/shaders/3D/triangle.vert");
    m_shader->add_source("../assets/shaders/3D/lighting.frag");
    m_shader->add_source("../assets/shaders/3D/triangle.frag");
    m_shader->compile();

    m_shadow_shader = std::make_shared<GLShader>();
    m_shadow_shader->add_source("../assets/shaders/3D/triangle.vert");
    m_shadow_shader->add_source("../assets/shaders/3D/shadow.frag");
    m_shadow_shader->compile();
}

void MeshRenderer::begin(glm::mat4& projectionview) const {
    m_shader->bind();
    m_shader->set_uniform("projectionview", projectionview);
    // TODO:
    m_shader->set_uniform("light_direction", glm::normalize(glm::vec3(0.0f, -1.0f, 0.0)));
    m_shader->set_uniform("light_color", glm::vec3(0.8f, 0.95f, 1.0f));
    m_shader->set_uniform("ambient_color", glm::vec3(0.2f));
}

void MeshRenderer::draw_mesh(Entity e) const {
    auto& mesh = e.get<Component::SimpleMesh>();
    auto& texture = e.get<Component::SimpleTexture2D>();
    auto& transform = e.get<Component::Transform>();
    m_shader->set_uniform("model", transform.get_matrix());
    m_shader->set_uniform("normalmatrix", transform.get_normalmatrix());
    m_shader->set_uniform("image", texture.texture);
    mesh.va.bind();
    glDrawElements(GL_TRIANGLES, mesh.va.index_count(), GL_UNSIGNED_INT, 0);
}

void MeshRenderer::end() const {
    GLVertexArray::unbind();
}

void MeshRenderer::begin_shadow(glm::mat4 &projectionview) const {
    m_shadow_shader->bind();
    m_shadow_shader->set_uniform("projectionview", projectionview);
}

void MeshRenderer::draw_shadow_mesh(Entity e) const {
    auto& mesh = e.get<Component::SimpleMesh>();
    auto& transform = e.get<Component::Transform>();
    m_shadow_shader->set_uniform("model", transform.get_matrix());
    mesh.va.bind();
    glDrawElements(GL_TRIANGLES, mesh.va.index_count(), GL_UNSIGNED_INT, 0);
}
