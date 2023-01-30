#pragma once

// ---Base----------------------------
#include <stdio.h>
#include "Base/Application.h"
#include "Base/Log.h"
#include "Base/Layer/Layer.h"
#include "Base/Input.h"
#include "Base/TimeStamp.h"

#include "Base/GlobalContext.h"

// ---Imgui----------------------------
#include "Base/ImGui/ImGuiLayer.h"

// ---Renderer-------------------------
#include "Core/Renderer/RenderCommand.h"
#include "Core/Renderer/Buffer.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SubTexture.h"
#include "Core/Renderer/Framebuffer.h"

// ---Scene----------------------------
#include "Core/Scene/Scene.h"
#include "Core/Scene/Components.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/ScriptableEntity.h"

// ---KeyCodes-------------------------
#include "Base/KeyCodes.h"
#include "Base/MouseButtonCodes.h"