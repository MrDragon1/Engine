#include "Skeleton.h"

namespace Ethereal {
Skeleton::Skeleton() {
    mRoot = Ref<Joint>::Create();
    mName = "Default Skeleton";
}

Skeleton::Skeleton(const Ref<Skeleton>& skel) {
    mRoot = skel->mRoot;
    mNameIDMap = skel->mNameIDMap;
    mName = skel->mName;
    mJointsMap = skel->mJointsMap;
}

void Skeleton::Load(const SkeletonDesc& desc) {
    Handle = desc.Handle;

    mName = desc.Name;
    mRoot = Ref<Joint>::Create();
    mRoot->Load(desc.RootJoint);
    mJointsMap.clear();
    mNameIDMap.clear();
    mJointsMap[mRoot->mID] = mRoot;
    mNameIDMap[mRoot->mName] = mRoot->mID;
    for (auto& child : mRoot->mChildren) {
        mJointsMap[child->mID] = child;
        mNameIDMap[child->mName] = child->mID;
    }
}

void Skeleton::Save(SkeletonDesc& desc) {
    desc.Handle = Handle;

    desc.Name = mName;
    mRoot->Save(desc.RootJoint);
}

void Skeleton::UpdatePose(AnimInterClip clip) {
    // Update LocalTranform in every joint
    for (auto state : clip.States) {
        Ref<Joint> node = mJointsMap[state.JointID];
        node->SetLocalTransform(state);
    }

    // Loop joint tree to update FinalBoneMatrices
    CalculateMatrices(mRoot, Matrix4::IDENTITY);
}

void Skeleton::CalculateMatrices(Ref<Joint> node, Matrix4 parentTransform) {
    Matrix4 globalTransform = parentTransform * node->GetLocalTransform();

    // InverseT transfroms vertex from model space to bone space
    mFinalBoneMatrices[node->mID] = mRoot->mInverseOffsetMatrix * globalTransform * node->mOffsetMatrix;

    for (auto& children : node->mChildren) {
        CalculateMatrices(children, globalTransform);
    }
}

}  // namespace Ethereal