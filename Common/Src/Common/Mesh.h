#pragma once

#include "Common/Shader.h"
#include "Common/Texture.h"
#include "Common/Vertex.h"
#include "Common/VertexBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Cm
{
    struct MeshTexture
    {
        Texture* Texture;
        aiTextureType Type;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures);
        void Draw(Shader& shader);

    private:
        std::vector<MeshTexture> m_Textures;
        VertexBuffer m_Vertices;
    };

    class Model
    {
        struct LoadedTexture
        {
            Texture Texture;
            std::string Name;
        };

    public:
        Model(const std::string_view& path);
        void Draw(Shader& shader);

    private:
        void LoadModel(const std::string_view& path);
        void ProcessNode(const std::string& directory, const aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(const std::string& directory, const aiMesh* mesh, const aiScene* scene);
        void LoadMaterialTextures(std::vector<MeshTexture>& meshTextures, const std::string& directory, aiMaterial* material, aiTextureType type);

    private:
        std::vector<Mesh> m_Meshes;
        std::vector<LoadedTexture> m_LoadedTextures;
    };
}
