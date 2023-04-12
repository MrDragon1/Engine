#pragma once

// ---Base----------------------------
#include <stdio.h>

#include "Base/Application.h"
#include "Base/GlobalContext.h"
#include "Base/Input.h"
#include "Base/Layer/Layer.h"
#include "Base/Log.h"
#include "Base/TimeStamp.h"

// ---Imgui----------------------------
#include "Base/ImGui/ImGuiLayer.h"

// ---Backend-------------------------
#include "Platform/Windows/Backend/Driver.h"

// ---Scene----------------------------
#include "Core/Scene/Components.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/ScriptableEntity.h"

// ---KeyCodes-------------------------
#include "Base/KeyCodes.h"
#include "Base/MouseButtonCodes.h"