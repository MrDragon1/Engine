#include "pch.h"
#include "LayerStack.h"

namespace Ethereal {

LayerStack::LayerStack() {}

LayerStack::~LayerStack() {
    for (Ref<Layer> layer : mLayers) layer.Reset();
}

void LayerStack::PushLayer(Ref<Layer> layer) {
    mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
    mLayerInsertIndex++;
}

void LayerStack::PushOverlay(Ref<Layer> overlay) { mLayers.emplace_back(overlay); }

void LayerStack::PopLayer(Ref<Layer> layer) {
    auto it = std::find(mLayers.begin(), mLayers.end(), layer);
    if (it != mLayers.end()) {
        mLayers.erase(it);
        mLayerInsertIndex--;
    }
}

void LayerStack::PopOverlay(Ref<Layer> overlay) {
    auto it = std::find(mLayers.begin(), mLayers.end(), overlay);
    if (it != mLayers.end()) mLayers.erase(it);
}

}  // namespace Ethereal