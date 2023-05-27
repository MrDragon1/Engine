#pragma once
#include "Core/Material/ShaderGenerator/ShaderNode.h"

namespace Ethereal {
using ShaderGraphPtr = Ref<class ShaderGraph>;

class ShaderGraph : public Asset {
   public:
    ShaderGraph(const string& name, DocumentPtr document, bool impl = false);

    void AddNode(ShaderNodePtr node);
    void AddOutputSocket(ShaderOutputSocketPtr input);
    void AddInputSocket(ShaderInputSocketPtr output);

    ShaderInputSocketPtr GetInputSocket(const string& name);
    unordered_map<string, ShaderInputSocketPtr>& GetInputSockets() { return mInputSockets; }
    ShaderOutputSocketPtr GetOutputSocket(const string& name);
    unordered_map<string, ShaderOutputSocketPtr>& GetOutputSockets() { return mOutputSockets; }
    ShaderNodePtr GetNode(const string& name);
    vector<ShaderNodePtr>& GetSortedNodes() { return mSortedNodes; }

    std::string GetName() const { return mName; }
    DocumentPtr GetDocument() { return mDocument; }
    bool IsImpl() { return mIsImpl; }
    void UpdateLink();
    void TopologicalSort();
    void UpdateVariable();

   private:
    string mName;
    unordered_map<string, ShaderNodePtr> mNodes;
    unordered_map<string, ShaderInputSocketPtr> mInputSockets;
    unordered_map<string, ShaderOutputSocketPtr> mOutputSockets;
    DocumentPtr mDocument;
    bool mIsImpl = false;
    vector<ShaderNodePtr> mSortedNodes;
};

}  // namespace Ethereal