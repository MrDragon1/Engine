#include "ShaderStage.h"
#include "Core/Material/ShaderGenerator/ShaderContext.h"
namespace Ethereal {
size_t VariableBlock::sBinding = 0;

void VariableBlock::Add(ShaderPortPtr port) {
    if (!mVariables.count(port->GetName())) {
        mVariables[port->GetName()] = port;
        mVariablesOrder.push_back(port);
    }
}

ShaderPortPtr VariableBlock::Add(ShaderNode* parent, const string& type, const string& name) {
    auto it = mVariables.find(name);
    if (it != mVariables.end()) {
        return it->second;
    }

    ShaderPortPtr port = ShaderPortPtr::Create(nullptr, type, name);
    mVariables[name] = port;
    mVariablesOrder.push_back(port);
    return port;
}

VariableBlockPtr ShaderStage::CreateUniformBlock(const string& name, const string& instance) {
    auto it = mUniforms.find(name);
    if (it == mUniforms.end()) {
        VariableBlockPtr b =
            VariableBlockPtr::Create(name, instance, VariableBlock::GenerateBinding());
        mUniforms[name] = b;
        return b;
    }
    return it->second;
}

VariableBlockPtr ShaderStage::CreateInputBlock(const string& name, const string& instance) {
    auto it = mInputs.find(name);
    if (it == mInputs.end()) {
        VariableBlockPtr b = VariableBlockPtr::Create(name, instance);
        mInputs[name] = b;
        return b;
    }
    return it->second;
}

VariableBlockPtr ShaderStage::CreateOutputBlock(const string& name, const string& instance) {
    auto it = mOutputs.find(name);
    if (it == mOutputs.end()) {
        VariableBlockPtr b = VariableBlockPtr::Create(name, instance);
        mOutputs[name] = b;
        return b;
    }
    return it->second;
}

VariableBlock& ShaderStage::GetUniformBlock(const string& name) {
    auto it = mUniforms.find(name);
    if (it == mUniforms.end()) {
        std::cout << name << " not found!" << std::endl;
    }
    return *it->second;
}

VariableBlock& ShaderStage::GetInputBlock(const string& name) {
    auto it = mInputs.find(name);
    if (it == mInputs.end()) {
        std::cout << name << " not found!" << std::endl;
    }
    return *it->second;
}

VariableBlock& ShaderStage::GetOutputBlock(const string& name) {
    auto it = mOutputs.find(name);
    if (it == mOutputs.end()) {
        std::cout << name << " not found!" << std::endl;
    }
    return *it->second;
}

void ShaderStage::AddFunctionDefinition(ShaderNodePtr node, ShaderContextPtr context) {
    node->EmitFunctionDefinition(context, this);
}

void ShaderStage::AddFunctionCall(ShaderNodePtr node, ShaderContextPtr context) {
    node->EmitFunctionCall(context, this);
}

void ShaderStage::EmitString(const string& str) { mCode += str; }

void ShaderStage::EmitComment(const string& str) { EmitLine("// " + str); }

void ShaderStage::EmitLine(const string& str) {
    BeginLine();
    EmitString(str);
    EndLine();
}

void ShaderStage::EmitVariableDeclaration(ShaderPortPtr data, ShaderContextPtr context,
                                          const string& prefix, bool initialvalue) {
    if (data->IsEmitted()) return;
    ValueBasePtr value = data->GetValue();
    if (!value) {
        EmitLine("Unknow Value");
        return;
    }
    string dv = value->GetSyntaxInitilizeString();
    string expression = (prefix.empty() ? "" : prefix + " ") + value->GetSyntaxString() + " " +
                        data->GetVariable(context->GetScope());
    if (initialvalue) expression += " = " + value->GetSyntaxString() + "(" + dv + ")";
    expression += ";";
    EmitLine(expression);
}

void ShaderStage::EmitGeometryConnection(ShaderPortPtr data, ShaderContextPtr context) {
    if (!data->IsGeom() || !data->IsSocket()) return;
    string expression =
        data->GetValue()->GetSyntaxString() + " " + data->GetVariable(context->GetScope()) + " = ";
    string geom = data->GetSource()->GetAttribute(MaterialAttribute::GEOM);

    if (GetName() == Stage::PIXEL) {
        auto& inputBlock = GetInputBlock(ShaderBuildInVariable::VERTEXDATA);
        if (geom == "normal") {
            expression +=
                "normalize(" +
                inputBlock[ShaderBuildInVariable::NORMAL_WORLD]->GetVariable(context->GetScope()) +
                ")";
        } else if (geom == "tangent") {
            expression +=
                "normalize(" +
                inputBlock[ShaderBuildInVariable::TANGENT_WORLD]->GetVariable(context->GetScope()) +
                ")";
        } else if (geom == "bitangent") {
            expression += "normalize(" +
                          inputBlock[ShaderBuildInVariable::BITANGENT_WORLD]->GetVariable(
                              context->GetScope()) +
                          ")";
        } else if (geom == "uv") {
            expression +=
                inputBlock[ShaderBuildInVariable::TEXCOORD]->GetVariable(context->GetScope());
        } else if (geom == "position") {
            expression +=
                inputBlock[ShaderBuildInVariable::POSITION_WORLD]->GetVariable(context->GetScope());
        }

        auto& privUniformBlock = GetUniformBlock(ShaderBuildInVariable::PRVUNIFORM);
        if (geom == "viewdir") {
            string pos =
                inputBlock[ShaderBuildInVariable::POSITION_WORLD]->GetVariable(context->GetScope());
            string viewpos = privUniformBlock[ShaderBuildInVariable::VIEW_POSITION]->GetVariable(
                context->GetScope());
            expression += "normalize(" + viewpos + " - " + pos + ")";
        } else if (geom == "lightdir") {
            string pos =
                inputBlock[ShaderBuildInVariable::POSITION_WORLD]->GetVariable(context->GetScope());
            string lightpos = privUniformBlock[ShaderBuildInVariable::LIGHT_POSITION]->GetVariable(
                context->GetScope());
            expression += "normalize(" + lightpos + " - " + pos + ")";
        }
    }
    EmitLine(expression + ";");
}

void ShaderStage::BeginLine() { EmitString(mIndention); }

void ShaderStage::EndLine() { EmitString("\n"); }

void ShaderStage::BeginScope() {
    EmitLine("{");
    mIndention += "    ";
}

void ShaderStage::EndScope(const string& endwith /*=EMPTY_STRING*/) {
    if (mIndention.size() < 4) {
        ET_CORE_WARN("Scope not match!");
        mIndention = "";
    } else {
        mIndention = mIndention.substr(0, mIndention.size() - 4);
    }
    EmitLine("}" + endwith);
}
}  // namespace Ethereal
