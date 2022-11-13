#include "Skeleton.h"

namespace Ethereal
{
    Skeleton::Skeleton() {
        m_Root = Ref<Joint>::Create();
        m_Name = "Default Skeleton";
    }

    Skeleton::Skeleton(const Ref<Skeleton>& skel) {
        m_Root = skel->m_Root;
        m_NameIDMap = skel->m_NameIDMap;
        m_Name = skel->m_Name;
        m_JointsMap = skel->m_JointsMap;
    }

    void Skeleton::UpdatePose(AnimInterClip clip) {
        // Update LocalTranform in every joint
        for (auto state : clip.States) {
            Ref<Joint> node = m_JointsMap[state.JointID];
            node->SetLocalTransform(state);
        }

        // Loop joint tree to update FinalBoneMatrices
        CalculateMatrices(m_Root, Matrix4x4::IDENTITY);
    }

    void Skeleton::CalculateMatrices(Ref<Joint> node, Matrix4x4 parentTransform) {
        Matrix4x4 globalTransform = parentTransform * node->GetLocalTransform();

        // InverseT transfroms vertex from model space to bone space
        m_FinalBoneMatrices[node->m_ID] = m_Root->m_InverseOffsetMatrix * globalTransform * node->m_OffsetMatrix;

        for (auto& children : node->m_Children) {
            CalculateMatrices(children, globalTransform);
        }
    }
}  // namespace Ethereal