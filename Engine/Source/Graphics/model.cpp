#include "model.h"
#include "util.h"

#include "Asset/asset.h"
#include "Graphics/mesh.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp> 
#include <glm/gtx/string_cast.hpp>
#include <spdlog/spdlog.h>

void Model::playAnimation(const std::string& animationPath, Assets& assets) {
    auto animation = loadAnimation(gAssets, animationPath, boneInfoMap);
    if (animation) {
        spdlog::info("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
        currentAnimation = animation;
    }
}

void Model::updateBoneTransform(float timeInSeconds) {
    if (!currentAnimation) {
        spdlog::error("No current animation");
        return;
    }

    // Calculate the current animation time
    float animationTime = fmod(timeInSeconds * currentAnimation->ticksPerSecond, currentAnimation->duration);
    if (animationTime < 0.0f) {
        animationTime += currentAnimation->duration;
    }

    //spdlog::info("Animation time: {}", animationTime);

    std::vector<glm::mat4> boneTransforms(boneCounter, glm::mat4(1.0f));

    // Function to recursively calculate bone transformations
    std::function<void(int, const glm::mat4&)> calculateBoneTransform = [&](int boneId, const glm::mat4& parentTransform) {
        const Bone& bone = currentAnimation->bones[boneId];

        glm::vec3 position = interpolatePosition(animationTime, bone);
        glm::quat rotation = interpolateRotation(animationTime, bone);
        glm::vec3 scale = interpolateScale(animationTime, bone);

        glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), position) *
            glm::toMat4(rotation) *
            glm::scale(glm::mat4(1.0f), scale);

        glm::mat4 globalTransform = parentTransform * localTransform;

        boneTransforms[bone.id] = globalTransform;

        spdlog::info("Bone ID: {}, Name: {}, Global Transform: [{}, {}, {}, {}]",
            bone.id, bone.name,
            globalTransform[0][0], globalTransform[0][1], globalTransform[0][2], globalTransform[0][3]);

        // Recursively calculate for children bones
        if (currentAnimation->boneHierarchy.find(bone.id) != currentAnimation->boneHierarchy.end()) {
            for (int childId : currentAnimation->boneHierarchy[bone.id]) {
                calculateBoneTransform(childId, globalTransform);
            }
        }
        };

    // Find the root bones and start the recursive calculation
    for (const auto& boneInfo : currentAnimation->boneHierarchy) {
        int parentId = boneInfo.first;
        bool isRoot = true;
        for (const auto& otherBoneInfo : currentAnimation->boneHierarchy) {
            if (std::find(otherBoneInfo.second.begin(), otherBoneInfo.second.end(), parentId) != otherBoneInfo.second.end()) {
                isRoot = false;
                break;
            }
        }
        if (isRoot) {
            calculateBoneTransform(parentId, glm::mat4(1.0f));
        }
    }

    // Store bone transforms for rendering
    this->boneTransforms = boneTransforms;
    spdlog::info("Updated bone transforms for model at animation time {}", animationTime);
}

void processNode(aiNode* node, const aiScene* scene, Model& model) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.push_back(processMesh(mesh, scene, model));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, model);
    }
}

Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Position
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // Texture Coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    extractBoneWeightForVertices(model, vertices, mesh, scene);

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", model);
    model.textures.insert(model.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", model);
    model.textures.insert(model.textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal", model);
    model.textures.insert(model.textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_height", model);
    model.textures.insert(model.textures.end(), heightMaps.begin(), heightMaps.end());

    return setupMesh(vertices, indices);
}

std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName, Model& model) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        std::string absolutePath = model.directory + "/" + path.C_Str();
        spdlog::info("Loading material texture {}", absolutePath);
        textures.push_back(*loadTexture(gAssets, absolutePath.c_str(), typeName));
    }

    return textures;
}

void setVertexBoneData(Vertex& vertex, int boneID, float weight) {
    for (int i = 0; i < 4; ++i)
    {
        if (vertex.boneIndices[i] < 0)
        {
            vertex.boneWeights[i] = weight;
            vertex.boneIndices[i] = boneID;
            break;
        }
    }
}

void extractBoneWeightForVertices(Model& model, std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (model.boneInfoMap.find(boneName) == model.boneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = model.boneCounter;
            newBoneInfo.offsetMatrix = convertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            model.boneInfoMap[boneName] = newBoneInfo;
            boneID = model.boneCounter;
            model.boneCounter++;
            spdlog::info("Extracting bone: {}", boneName);
            spdlog::info("Bone {} with ID {} and offset matrix:\n{}", boneName, boneID, glm::to_string(newBoneInfo.offsetMatrix));
        }
        else {
            boneID = model.boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId < vertices.size());
            setVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}
