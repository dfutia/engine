#include "animation.h"
#include "util.h"
#include "bone.h"
#include "model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Bone* Animation::findBone(const std::string& name) {
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone& bone) {
            return bone.GetBoneName() == name;
        });
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}

void Animation::readMissingBones(const aiAnimation* animation) {
    int size = animation->mNumChannels;

    for (int i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;
        m_Bones.push_back(Bone(channel->mNodeName.data, i, channel));
    }
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);
    dest.name = src->mName.data;
    dest.transformation = convertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (unsigned i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        readHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}