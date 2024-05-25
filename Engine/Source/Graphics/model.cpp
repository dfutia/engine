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

    std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", scene, model);
    model.textures.insert(model.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", scene, model);
    model.textures.insert(model.textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal", scene, model);
    model.textures.insert(model.textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_height", scene, model);
    model.textures.insert(model.textures.end(), heightMaps.begin(), heightMaps.end());

    return setupMesh(vertices, indices);
}

// https://chatgpt.com/g/g-3s6SJ5V7S-askthecode-git-companion/c/7ce512cd-ffa7-43f6-97ab-fdb33385d32c?oauth_success=true
std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene, Model& model) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);

        aiTexture const* texture = scene->GetEmbeddedTexture(path.C_Str());
        if (texture) {
            unsigned int textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height;
            unsigned char* data = nullptr;
            if (texture->mHeight == 0) { // Compressed texture
                data = stbi_load_from_memory((unsigned char*)texture->pcData, texture->mWidth, &width, &height, nullptr, 4);
            }
            else { // Uncompressed texture
                width = texture->mWidth;
                height = texture->mHeight;
                data = (unsigned char*)texture->pcData;
            }

            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                if (texture->mHeight == 0) {
                    stbi_image_free(data);
                }
            }
            else {
                spdlog::error("Failed to load embedded texture");
                if (texture->mHeight == 0) {
                    stbi_image_free(data);
                }
                glDeleteTextures(1, &textureID);
                return {};
            }

            auto texture = std::make_shared<Texture>();
            texture->id = textureID;
            texture->type = type;

            spdlog::info("Embedded texture loaded");

            textures.push_back(*texture);
        }
        else {
            spdlog::info("external  texture");
            std::string absolutePath = model.directory + "/" + path.C_Str();
            textures.push_back(*loadTexture(gAssets, absolutePath.c_str(), typeName));
        }
    }

    return textures;
}