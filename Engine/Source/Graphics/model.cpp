#include "model.h"

#include "Asset/asset.h"
#include "Graphics/mesh.h"

#include <glad/glad.h>

#include <spdlog/spdlog.h>

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
        textures.push_back(loadTexture(gAssets, absolutePath.c_str()));
    }

    return textures;
}