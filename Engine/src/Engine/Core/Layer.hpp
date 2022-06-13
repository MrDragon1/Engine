#pragma once

#include "Engine/Core/Utils.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Core/Timestep.hpp"
namespace Engine {

class Layer {
  public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(Timestep ts) {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event& event) {}

    inline const std::string& GetName() const { return m_DebugName; }

  protected:
    std::string m_DebugName;
};

}  // namespace Engine