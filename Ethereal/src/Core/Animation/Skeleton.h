#pragma once
#include "Core/Animation/Joint.h"
#include "Core/Animation/Animation.h"

namespace Ethereal
{
    // The skeleton that describes how the joints connect
    class Skeleton : public Asset {
      public:
        Skeleton();
        Skeleton(const Ref<Skeleton>& skel);
        void UpdatePose(AnimInterClip clip);
        size_t GetJointID(std::string name) {
            if (m_NameIDMap.find(name) == m_NameIDMap.end()) {
                ET_CORE_WARN("Unknow joint name {0}", name);
                return -1;
            }
            return m_NameIDMap[name];
        }
        std::unordered_map<size_t, Matrix4x4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

        static AssetType GetStaticType() { return AssetType::Skeleton; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

      public:
        Ref<Joint> m_Root;
        std::unordered_map<size_t, Ref<Joint>> m_JointsMap;
        std::unordered_map<std::string, size_t> m_NameIDMap;
        std::unordered_map<size_t, Matrix4x4> m_FinalBoneMatrices;
        std::string m_Name;

      private:
        void CalculateMatrices(Ref<Joint> node, Matrix4x4 parentTransform);
    };

}  // namespace Ethereal
