#include "Joint.h"

namespace Ethereal
{
    void Joint::Load(const JointDesc& desc) {
        m_Name = desc.Name;
        m_ID = desc.ID;
        m_InitialPosition = desc.InitialPosition;
        m_InitialRotation = desc.InitialRotation;
        m_InitialScale = desc.InitialScale;

        m_OffsetMatrix = desc.OffsetMatrix;
        m_InverseOffsetMatrix = desc.InverseOffsetMatrix;
        m_LocalTransform = desc.LocalTransform;

        m_Children.clear();
        for(auto child : desc.Children) {
            Ref<Joint> joint = Ref<Joint>::Create();
            joint->Load(child);
            joint->m_Parent = this;
            m_Children.push_back(joint);
        }
    }

    void Joint::Save(JointDesc& desc) {
        desc.Name = m_Name;
        desc.ID = m_ID;
        desc.InitialPosition = m_InitialPosition;
        desc.InitialRotation = m_InitialRotation;
        desc.InitialScale = m_InitialScale;

        desc.OffsetMatrix = m_OffsetMatrix;
        desc.InverseOffsetMatrix = m_InverseOffsetMatrix;
        desc.LocalTransform = m_LocalTransform;

        desc.Children.clear();
        for(auto child : m_Children) {
            JointDesc childDesc;
            child->Save(childDesc);
            desc.Children.push_back(childDesc);
        }
    }


    void Joint::SetLocalTransform(AnimState state) {
        auto position = Math::Translate(Matrix4::IDENTITY, state.PositionState.Position);
        auto rotation = Math::Rotate(Matrix4::IDENTITY, state.RotationState.Rotation);
        auto scale = Math::Scale(Matrix4::IDENTITY, state.ScaleState.Scale);
        m_LocalTransform = position * rotation * scale;
    }
}  // namespace Ethereal