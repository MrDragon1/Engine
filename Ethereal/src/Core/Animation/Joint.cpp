#include "Joint.h"

namespace Ethereal
{
    void Joint::Load(const JointDesc& desc) {
        mName = desc.Name;
        mID = desc.ID;
        mInitialPosition = desc.InitialPosition;
        mInitialRotation = desc.InitialRotation;
        mInitialScale = desc.InitialScale;

        mOffsetMatrix = desc.OffsetMatrix;
        mInverseOffsetMatrix = desc.InverseOffsetMatrix;
        mLocalTransform = desc.LocalTransform;

        mChildren.clear();
        for(auto child : desc.Children) {
            Ref<Joint> joint = Ref<Joint>::Create();
            joint->Load(child);
            joint->mParent = this;
            mChildren.push_back(joint);
        }
    }

    void Joint::Save(JointDesc& desc) {
        desc.Name = mName;
        desc.ID = mID;
        desc.InitialPosition = mInitialPosition;
        desc.InitialRotation = mInitialRotation;
        desc.InitialScale = mInitialScale;

        desc.OffsetMatrix = mOffsetMatrix;
        desc.InverseOffsetMatrix = mInverseOffsetMatrix;
        desc.LocalTransform = mLocalTransform;

        desc.Children.clear();
        for(auto child : mChildren) {
            JointDesc childDesc;
            child->Save(childDesc);
            desc.Children.push_back(childDesc);
        }
    }


    void Joint::SetLocalTransform(AnimState state) {
        auto position = Math::Translate(Matrix4::IDENTITY, state.PositionState.Position);
        auto rotation = Math::Rotate(Matrix4::IDENTITY, state.RotationState.Rotation);
        auto scale = Math::Scale(Matrix4::IDENTITY, state.ScaleState.Scale);
        mLocalTransform = position * rotation * scale;
    }
}  // namespace Ethereal