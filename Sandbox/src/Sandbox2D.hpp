#pragma once

#include "Engine.hpp"
#include "ParticleSystem.hpp"
class Sandbox2D : public Engine::Layer {
  public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Engine::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Engine::Event& e) override;

  private:
    Engine::OrthographicCameraController m_CameraController;
    Engine::Ref<Engine::Texture2D> m_CheckerboardTexture;

    // Temp
    Engine::Ref<Engine::VertexArray> m_SquareVA;
    Engine::Ref<Engine::Shader> m_FlatColorShader;

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};
};