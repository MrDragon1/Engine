#pragma once
#include "Core/Material/ShaderGenerator/ShaderStage.h"
#include "Backend/Driver.h"
#include "Utils/Facotry.h"

namespace Ethereal {
using ShaderGeneratorPtr = Ref<class ShaderGenerator>;
using ShaderPtr = Ref<class Shader>;

class Shader : public RefCounted {
   public:
    Shader(const string& name) : mName(name) {}
    ShaderStagePtr CreateStage(const string& name);

    ShaderStagePtr GetStage(const string& name);
    unordered_map<string, ShaderStagePtr> GetStages() { return mStages; }
    void Compile();

    ProgramHandle GetProgram() { return mProgram; }

   protected:
    string mName;
    unordered_map<string, ShaderStagePtr> mStages;

    ProgramHandle mProgram;
    friend class ShaderGenerator;
};

class ShaderGenerator : public RefCounted {
   public:
    ShaderGenerator();
    // create necessary variables, eg. vertex input & pixel input
    void CreateVariables(ShaderGraphPtr graph, ShaderContextPtr context, ShaderPtr shader);
    void EmitVertexStage(ShaderGraphPtr graph, ShaderContextPtr context, ShaderStagePtr stage);
    void EmitPixelStage(ShaderGraphPtr graph, ShaderContextPtr context, ShaderStagePtr stage);

    void AddConnectorBlock(const string& name, const string& instance, ShaderStagePtr src,
                           ShaderStagePtr dst);
    void AddConnectorVariable(const string& name, const string& type, const string& variable,
                              ShaderStagePtr src, ShaderStagePtr dst);

    void EmitTypeDefinition(ShaderContextPtr context, ShaderStagePtr stage);

    void EmitFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context, ShaderStagePtr stage);

    void EmitFunctionDefinitions(ShaderGraphPtr graph, ShaderContextPtr context,
                                 ShaderStagePtr stage);

    void EmitFunctionCall(ShaderNodePtr node, ShaderContextPtr context, ShaderStagePtr stage,
                          bool checkScope = true);

    void EmitFunctionCalls(ShaderGraphPtr graph, ShaderContextPtr context, ShaderStagePtr stage,
                           uint32_t classification = 0u);

    void EmitDependentFunctionCalls(ShaderNodePtr node, ShaderContextPtr context,
                                    ShaderStagePtr stage, uint32_t classification = 0u);

    void EmitLightData(ShaderContextPtr context, ShaderStagePtr stage);

    ShaderNodeImplPtr GetImpl(NodeInstancePtr nodeDefine, ShaderContextPtr context);

   public:
    void EmitUniforms(ShaderContextPtr context, ShaderStagePtr stage);
    void EmitOutputs(ShaderContextPtr context, ShaderStagePtr stage);
    void EmitInputs(ShaderContextPtr context, ShaderStagePtr stage);
    void EmitVersion(ShaderContextPtr context, ShaderStagePtr stage);
    void EmitLibs(ShaderContextPtr context, ShaderStagePtr stage);

   private:
    template <class T>
    using CreatorFunction = Ref<T> (*)();
    void RegisterImplementation(const string& name, CreatorFunction<ShaderNodeImpl> func);
    void RegisterImplementation(const StringVec& nameVec, CreatorFunction<ShaderNodeImpl> func);

   private:
    Factory<ShaderNodeImpl> mImplFactory;
};

}  // namespace Ethereal
