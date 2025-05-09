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
    class Mesh
    {
    public:
        enum class TextureType
        {
            Phong_Diffuse,
            Phong_Specular,
            Phong_Emission,
        };

        struct TextureEntry
        {
            Texture* Texture;
            TextureType Type;
        };

    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<TextureEntry>& textures, VertexBuffer::Type vertexDataType = VertexBuffer::Static);
        ~Mesh();

        const VertexBuffer& GpuVertexBuffer() const
        {
            return m_VertexBuffer;
        }

        const std::vector<TextureEntry>& Textures() const
        {
            return m_Textures;
        }

        void Draw(Shader& shader);

    private:
        VertexBuffer m_VertexBuffer;
        std::vector<TextureEntry> m_Textures;
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
        ~Model();

        void Draw(Shader& shader);

    private:
        void LoadModel(const std::string_view& path);
        void ProcessNode(const std::string& directory, const aiNode* node, const aiScene* scene);
        void ProcessMesh(const std::string& directory, const aiMesh* mesh, const aiScene* scene);
        void LoadMaterialTextures(const std::string& directory, const aiMaterial* mat, aiTextureType type, std::vector<Mesh::TextureEntry>& out);

    private:
        std::vector<Mesh> m_Meshes;
        std::vector<LoadedTexture> m_LoadedTextures;
    };
}
