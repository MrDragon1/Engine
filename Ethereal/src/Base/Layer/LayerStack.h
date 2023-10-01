#pragma once
#include "Utils/EngineMarco.h"
#include "Layer.h"

#include <vector>

namespace Ethereal {

class LayerStack {
   public:
    LayerStack();
    ~LayerStack();

    void PushLayer(Ref<Layer> layer);
    void PushOverlay(Ref<Layer> overlay);
    void PopLayer(Ref<Layer> layer);
    void PopOverlay(Ref<Layer> overlay);

    std::vector<Ref<Layer>>::iterator begin() { return mLayers.begin(); }
    std::vector<Ref<Layer>>::iterator end() { return mLayers.end(); }

    std::vector<Ref<Layer>>::reverse_iterator rbegin() { return mLayers.rbegin(); }
    std::vector<Ref<Layer>>::reverse_iterator rend() { return mLayers.rend(); }

    std::vector<Ref<Layer>>::const_iterator begin() const { return mLayers.begin(); }
    std::vector<Ref<Layer>>::const_iterator end() const { return mLayers.end(); }
    std::vector<Ref<Layer>>::const_reverse_iterator rbegin() const { return mLayers.rbegin(); }
    std::vector<Ref<Layer>>::const_reverse_iterator rend() const { return mLayers.rend(); }

   private:
    std::vector<Ref<Layer>> mLayers;
    unsigned int mLayerInsertIndex = 0;
};

}  // namespace Ethereal