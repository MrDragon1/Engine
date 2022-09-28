#include "Skeleton.h"

namespace Ethereal
{
    Skeleton::Skeleton() {}

    void Skeleton::UpdatePose(AnimInterClip clip) {
        // Update LocalTranform in every joint
        for (auto state : clip.States) {
            Ref<Joint> node = m_JointsMap[state.JointID];
            node->SetLocalTransform(state.States[0]);
        }

        // Loop joint tree to update FinalBoneMatrices
        CalculateMatrices(m_Root, glm::mat4(1.0f));
    }

    void Skeleton::CalculateMatrices(Ref<Joint> node, glm::mat4 parentTransform) {
        glm::mat4 globalTransform = parentTransform * node->GetLocalTransform();

        // InverseT transfroms vertex from model space to bone space
        m_FinalBoneMatrices[node->m_ID] = globalTransform * node->m_InverseT;

        for (auto& children : node->m_Children) {
            CalculateMatrices(children, globalTransform);
        }
    }
}  // namespace Ethereal