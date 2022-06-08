#include "Engine.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Platform/Linux/OpenGL/OpenGLShader.hpp"
#include "imgui.h"
class ExampleLayer : public Engine::Layer {
  public:
    ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f) {
        m_VertexArray.reset(Engine::VertexArray::Create());

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
            0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f};

        Engine::Ref<Engine::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));
        vertexBuffer->SetLayout({
            {Engine::ShaderDataType::Float3, "a_Position"},
            {Engine::ShaderDataType::Float4, "a_Color"},
        });
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = {0, 1, 2};
        Engine::Ref<Engine::IndexBuffer> indexBuffer;
        indexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA.reset(Engine::VertexArray::Create());

        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

        Engine::Ref<Engine::VertexBuffer> squareVB;
        squareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
            {Engine::ShaderDataType::Float3, "a_Position"},
            {Engine::ShaderDataType::Float2, "a_TexCoord"},
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
        Engine::Ref<Engine::IndexBuffer> squareIB;
        squareIB.reset(Engine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
			}
		)";

        std::string fragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

        m_Shader = Engine::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

        std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
			}
		)";

        std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
            uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

        m_FlatColorShader = Engine::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

        auto textureShader = m_ShaderLibrary.Load("../../../../Sandbox/assets/shaders/Texture.glsl");
        m_Texture = Engine::Texture2D::Create("../../../../Sandbox/assets/textures/Checkerboard.png");

        std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
        std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
    }

    void OnUpdate(Engine::Timestep ts) override {
        if (Engine::Input::IsKeyPressed(ENGINE_KEY_LEFT))
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        else if (Engine::Input::IsKeyPressed(ENGINE_KEY_RIGHT))
            m_CameraPosition.x += m_CameraMoveSpeed * ts;

        if (Engine::Input::IsKeyPressed(ENGINE_KEY_UP))
            m_CameraPosition.y += m_CameraMoveSpeed * ts;
        else if (Engine::Input::IsKeyPressed(ENGINE_KEY_DOWN))
            m_CameraPosition.y -= m_CameraMoveSpeed * ts;

        if (Engine::Input::IsKeyPressed(ENGINE_KEY_A))
            m_CameraRotation += m_CameraRotationSpeed * ts;
        else if (Engine::Input::IsKeyPressed(ENGINE_KEY_D))
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        Engine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Engine::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Engine::Renderer::BeginScene(m_Camera);

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->Bind();
        std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

            for (int y = 0; y < 20; y++) {
                    for (int x = 0; x < 20; x++) {
                        glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                        Engine::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
                    }
            }

        // Engine::Renderer::Submit(m_Shader, m_VertexArray);

        auto textureShader = m_ShaderLibrary.Get("Texture");
        m_Texture->Bind();

        Engine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        Engine::Renderer::EndScene();
    }

    void OnEvent(Engine::Event& event) override {
    }

    void OnImGuiRender() override {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

  private:
    Engine::ShaderLibrary m_ShaderLibrary;
    Engine::Ref<Engine::Shader> m_Shader;
    Engine::Ref<Engine::VertexArray> m_VertexArray;

    Engine::Ref<Engine::Shader> m_FlatColorShader;
    Engine::Ref<Engine::VertexArray> m_SquareVA;

    Engine::Ref<Engine::Texture2D> m_Texture;

    Engine::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 1.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 180.0f;

    glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};
class Sandbox : public Engine::Application {
  public:
    Sandbox() { PushLayer(new ExampleLayer()); }

    ~Sandbox() {}
};

Engine::Application* Engine::CreateApplication() { return new Sandbox(); }
