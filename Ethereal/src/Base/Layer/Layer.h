#pragma once

#include "Base/Utils.h"
#include "Base/Event/Event.h"
#include "Base/TimeStamp.h"
namespace Ethereal
{

    class Layer {
      public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(TimeStamp ts) {}
        virtual void OnImGuiRender() {}
    virtual void OnEvent(Event& event) {}

    inline const std::string& GetName() const { return m_DebugName; }

  protected:
    std::string m_DebugName;
};

}  // namespace Ethereal