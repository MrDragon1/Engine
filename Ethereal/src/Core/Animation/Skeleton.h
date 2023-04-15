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
        Skeleton(const SkeletonDesc& desc) { Load(desc); }
        void Load(const SkeletonDesc& desc);
        void Save(SkeletonDesc& desc);

        void UpdatePose(AnimInterClip clip);
        size_t GetJointID(std::string name) {
            if (mNameIDMap.find(name) == mNameIDMap.end()) {
                ET_CORE_WARN("Unknow joint name {0}", name);
                return -1;
            }
            return mNameIDMap[name];
        }
        std::unordered_map<size_t, Matrix4> GetFinalBoneMatrices() { return mFinalBoneMatrices; }

        static AssetType GetStaticType() { return AssetType::Skeleton; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

      public:
        Ref<Joint> mRoot;
        std::unordered_map<size_t, Ref<Joint>> mJointsMap;
        std::unordered_map<std::string, size_t> mNameIDMap;
        std::unordered_map<size_t, Matrix4> mFinalBoneMatrices;
        std::string mName;

      private:
        void CalculateMatrices(Ref<Joint> node, Matrix4 parentTransform);
    };

}  // namespace Ethereal
