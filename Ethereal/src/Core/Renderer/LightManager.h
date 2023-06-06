#pragma once
#include "Core/Renderer/Environment.h"

#include "Core/Material/MaterialCore.h"
namespace Ethereal {

struct DirectionalLight {
    Vector3 mDirection;
    Color3 mColor;
    float mIntensity;
};

struct PointLight {
    Vector3 mPosition;
    Color3 mColor;
    float mIntensity;
};

class LightManager {
   public:
    void SetEnvironment(EnvironmentPtr env) { mEnvironment = env; }
    void AddDirectionalLight(const DirectionalLight& light) { mDirectionalLights.push_back(light); }
    void AddPointLight(const PointLight& light) { mPointLights.push_back(light); }

    EnvironmentPtr GetEnvironment() { return mEnvironment; }

    void Bind(MaterialCorePtr mat);
    void Clear();

   private:
    EnvironmentPtr mEnvironment;
    std::vector<DirectionalLight> mDirectionalLights;
    std::vector<PointLight> mPointLights;
};

}  // namespace Ethereal
