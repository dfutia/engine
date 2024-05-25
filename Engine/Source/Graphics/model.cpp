#include "model.h"
#include "util.h"

#include "Asset/asset.h"
#include "Graphics/mesh.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp> 
#include <glm/gtx/string_cast.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>

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
        setVertexBoneDataToDefault(vertex);
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.color = glm::vec3(1.0f); // Default color
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
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

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", model);
    model.textures.insert(model.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", model);
    model.textures.insert(model.textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal", model);
    model.textures.insert(model.textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_height", model);
    model.textures.insert(model.textures.end(), heightMaps.begin(), heightMaps.end());

    extractBoneWeightForVertices(vertices, mesh, scene, model);

    return setupMesh(vertices, indices);
}

std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName, Model& model) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        std::string absolutePath = model.directory + "/" + path.C_Str();
        //spdlog::info("Loading material texture {}", absolutePath);
        textures.push_back(*loadTexture(gAssets, absolutePath.c_str(), typeName));
    }

    return textures;
}

void setVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < 4; i++)
    {
        vertex.boneIds[i] = -1;
        vertex.boneWeights[i] = 0.0f;
    }
}

void setVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < 4; ++i)
    {
        if (vertex.boneIds[i] < 0)
        {
            vertex.boneWeights[i] = weight;
            vertex.boneIds[i] = boneID;
            break;
        }
    }
}

void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene, Model& model)
{
    auto& boneInfoMap = model.boneInfoMap;
    int& boneCount = model.boneCounter;

    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        //spdlog::info("Bone Name: {}", boneName);
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offsetMatrix = convertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            setVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}