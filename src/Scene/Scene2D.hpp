#pragma once

#include <cmath>

#include "AbstractScene.hpp"
#include "callbacks.hpp"
#include "renderer/Renderer2D.hpp"
#include "camera/Camera2D.hpp"

// TODO: 
// Move definitions to cpp file

class Scene2D : public AbstractScene {
private:
    struct ScreenShake {
        float intensity = 0.03f;
        float damping = 5.0f;
        float frequency = 50.0f;

        void reset() {
            m_time = -0.05f;
        }

        void update(float delta_time) {
            m_time += delta_time;
        }

        float get_offset() {
            m_last_offset = m_time > 10 * damping ? 0.0f : intensity * sin(frequency * m_time) * std::min(1.0f, exp(-m_time * damping));
            return m_last_offset;
        }

        float get_delta() {
            float last = m_last_offset;
            return get_offset() - last;
        }

    private:
        float m_time = 10000000.0f;
        float m_last_offset = 0.0f;
    };

    Renderer2D m_renderer;
    Camera2D m_camera;
    ScreenShake m_shake;

public:

    Scene2D() : 
        m_renderer(Renderer2D()), 
        m_camera(Camera2D(-1.0f, 1.0f, -1.0f, 1.0f))
    {}

    void init() {
        m_renderer.init();
    }

    // Entity Constructors:

    Entity create_circle() {
        return create_circle(glm::vec3(0), 0.1f);
    }

    Entity create_circle(glm::vec3 pos, float r, glm::vec4 color = glm::vec4(0.8, 0.3, 0, 1)) {
        return create_circle("Circle Entity", pos, r, color);
    };

    Entity create_circle(std::string name, glm::vec3 pos, float r, glm::vec4 color = glm::vec4(0.8, 0.3, 0, 1)) {
        Entity entity = create_entity(name);

        entity.add<Component::Circle>(color); 
        entity.add<Component::Transform>(pos, glm::vec3(r, r, 1));

        return entity;
    };

    Entity create_quad() {
        return create_quad(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f));
    }

    Entity create_quad(glm::vec3 position, glm::vec2 size, glm::vec4 color = glm::vec4(0.4, 0.7, 0, 1)) {
        return create_quad("Quad Entity", position, size, color);
    }

    Entity create_quad(std::string name, glm::vec3 position, glm::vec2 size, glm::vec4 color = glm::vec4(0.4, 0.7, 0, 1)) {
        Entity entity = create_entity(name);
    
        entity.add<Component::Quad>(color); 
        entity.add<Component::Transform>(position, glm::vec3(size, 1)); 

        return entity;
    }

    void screen_shake() { m_shake.reset(); }

    // Systems

    void resolve_on_update() {
        auto view = m_registry.view<Component::OnUpdate>();
        for (entt::entity e : view) {
            Entity ent = Entity(m_registry, e);
            m_registry.get<Component::OnUpdate>(e).callback(ent);
        }
    }

    void resolve_scheduled_deletes() {
        auto view = m_registry.view<Component::ScheduledDelete>();
        m_registry.destroy(view.begin(), view.end());
    }

    void update(float delta_time) {
        m_shake.update(delta_time);
        resolve_on_update();
        resolve_scheduled_deletes();
    }

    void render(glm::vec2 resolution) {
        // Fixing shorter dimension here to avoid edges +-1 being outside a standard window
        float aspect = resolution.x / resolution.y;
        if (aspect > 1)
            m_camera.set_bounds(-aspect, aspect, -1.0f, 1.0f);
        else
            m_camera.set_bounds(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect);

        float shake_delta = m_shake.get_delta();
        m_camera.translate_by(glm::vec3(0.0f, shake_delta, 0.0f));

        m_renderer.begin(m_camera.m_projectionview, resolution);

        {
            auto view = m_registry.view<Component::Transform, Component::Quad>();
            for (auto entity : view) {
                auto [transform, quad] = view.get<Component::Transform, Component::Quad>(entity);
                m_renderer.draw_quad(transform.position, transform.scale, quad.color);
            }
        }

        {
            auto view = m_registry.view<Component::Transform, Component::Circle>();
            for (auto entity : view) {
                auto [transform, circle] = view.get<Component::Transform, Component::Circle>(entity);
                // TODO: should use scale directly
                // ... or transform matrix
                m_renderer.draw_circle(transform.position, transform.scale.x, circle.color); 
            }
        }

        m_renderer.end();
    }

};