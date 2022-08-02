#pragma once

#include "Core/Utils.h"
#include "Event/Event.h"
#include "Core/Timestep.h"
namespace Ethereal {

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

}  // namespace Ethereal