#pragma once

// ---Core----------------------------
#include <stdio.h>
#include "Ethereal/Core/Application.hpp"
#include "Ethereal/Core/Log.hpp"
#include "Ethereal/Core/Layer.hpp"
#include "Ethereal/Core/Input.hpp"
#include "Ethereal/Core/OrthographicCameraController.hpp"
#include "Ethereal/Core/Timestep.hpp"

#include "Ethereal/Core/GlobalContext.hpp"

// ---Imgui----------------------------
#include "Ethereal/ImGui/ImGuiLayer.hpp"


// ---Renderer-------------------------
#include "Ethereal/Renderer/RenderCommand.hpp"
#include "Ethereal/Renderer/Buffer.hpp"
#include "Ethereal/Renderer/VertexArray.hpp"
#include "Ethereal/Renderer/Shader.hpp"
#include "Ethereal/Renderer/OrthographicCamera.hpp"
#include "Ethereal/Renderer/Texture.hpp"
#include "Ethereal/Renderer/SubTexture.hpp"
#include "Ethereal/Renderer/Framebuffer.hpp"

// ---Scene----------------------------
#include "Ethereal/Scene/Scene.hpp"
#include "Ethereal/Scene/Components.hpp"
#include "Ethereal/Scene/Entity.hpp"
#include "Ethereal/Scene/ScriptableEntity.hpp"

// ---KeyCodes-------------------------
#include "Ethereal/Core/KeyCodes.hpp"
#include "Ethereal/Core/MouseButtonCodes.hpp"