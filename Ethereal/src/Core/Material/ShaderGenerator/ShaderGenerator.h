#pragma once
#include "Core/Material/ShaderGenerator/ShaderStage.h"
#include "Backend/Driver.h"
namespace Ethereal {
using ShaderGeneratorPtr = Ref<class ShaderGenerator>;
using ShaderPtr = Ref<class Shader>;

class Shader : public RefCounted {
   public:
    Shader(const string& name, ShaderGraphPtr graph) : mName(name), mGraph(graph) {}
    ShaderStagePtr CreateStage(const string& name);

    ShaderStagePtr GetStage(const string& name);
    unordered_map<string, ShaderStagePtr> GetStages() { return mStages; }
    void Compile();

    ProgramHandle GetProgram() { return mProgram; }

   protected:
    string mName;
    ShaderGraphPtr mGraph;
    unordered_map<string, ShaderStagePtr> mStages;

    ProgramHandle mProgram;
    friend class ShaderGenerator;
};

class ShaderGenerator : public RefCounted {
   public:
    // create necessary variables, eg. vertex input & pixel input
    void CreateVariables(ShaderGraphPtr graph, ShaderContextPtr context, ShaderPtr shader);
    void EmitVertexStage(ShaderGraphPtr graph, ShaderContextPtr context, ShaderStagePtr stage);
    void EmitPixelStage(ShaderGraphPtr graph, ShaderContextPtr context, ShaderStagePtr stage);

    void AddConnectorBlock(const string& name, const string& instance, ShaderStagePtr src,
                           ShaderStagePtr dst);
    void AddConnectorVariable(const string& name, const string& type, const string& variable,
                              ShaderStagePtr src, ShaderStagePtr dst);

    void EmitTypeDefinition(ShaderContextPtr context, ShaderStagePtr stage);

   public:
    void EmitUniforms(ShaderContextPtr context, ShaderStagePtr stage);
    void EmitOutputs(ShaderContextPtr context, ShaderStagePtr stage);
    void EmitInputs(ShaderContextPtr context, ShaderStagePtr stage);
    void EmitVersion(ShaderContextPtr context, ShaderStagePtr stage);
};

}  // namespace Ethereal
