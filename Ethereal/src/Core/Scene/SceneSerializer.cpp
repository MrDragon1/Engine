#include "pch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "Core/Asset/AssetManager.h"
#include "Utils/YAMLSerializationHelpers.h"

#include <fstream>
#include "Utils/Yaml.h"

namespace Ethereal {
static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType) {
    switch (bodyType) {
        case Rigidbody2DComponent::BodyType::Static:
            return "Static";
        case Rigidbody2DComponent::BodyType::Dynamic:
            return "Dynamic";
        case Rigidbody2DComponent::BodyType::Kinematic:
            return "Kinematic";
    }

    ET_CORE_ASSERT(false, "Unknown body type");
    return {};
}

static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString) {
    if (bodyTypeString == "Static") return Rigidbody2DComponent::BodyType::Static;
    if (bodyTypeString == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
    if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

    ET_CORE_ASSERT(false, "Unknown body type");
    return Rigidbody2DComponent::BodyType::Static;
}

SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}

}  // namespace Ethereal