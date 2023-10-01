#pragma once
#include "Base/Event/Event.h"
#include "Utils/EngineMarco.h"
#include "Base/TimeStamp.h"
namespace Ethereal {

class Layer : public RefCounted {
   public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(TimeStamp ts) {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event& event) {}

    inline const std::string& GetName() const { return mDebugName; }

   protected:
    std::string mDebugName;
};

}  // namespace Ethereal