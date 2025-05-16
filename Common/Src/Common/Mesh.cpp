#include "Common/Mesh.h"

namespace Cm
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures)
        : m_Textures(textures)
    {
        m_Vertices.PushData(vertices);
        m_Vertices.PushData(indices);
    }

    Mesh::Mesh(size_t vertexCount, const Vertex* vertices, size_t indicesCount, const unsigned int* indices, const std::vector<MeshTexture>& textures)
        : m_Textures(textures)
    {
        m_Vertices.PushData(vertexCount, vertices);
        m_Vertices.PushData(indicesCount, indices);
    }

    void Mesh::Draw(Shader& shader) const
    {
        // Material Setup
        unsigned int diffuseCount = 0;
        unsigned int specularCount = 0;
        unsigned int emissionCount = 0;
        for (unsigned int i = 0; i < m_Textures.size(); ++i)
        {
            const MeshTexture& texture = m_Textures[i];
            constexpr unsigned int uniformNameMaxSize = 128;
            char uniformName[uniformNameMaxSize];

            // Support PBR in the future
            switch (texture.Type)
            {
            case aiTextureType_DIFFUSE:
            case aiTextureType_BASE_COLOR:
                snprintf(uniformName, uniformNameMaxSize, "u_Material.DiffuseMaps[%u]", diffuseCount);
                ++diffuseCount;
                break;
            case aiTextureType_SPECULAR:
            case aiTextureType_METALNESS:
                snprintf(uniformName, uniformNameMaxSize, "u_Material.SpecularMaps[%u]", specularCount);
                ++specularCount;
                break;
            case aiTextureType_EMISSIVE:
            case aiTextureType_EMISSION_COLOR:
                snprintf(uniformName, uniformNameMaxSize, "u_Material.EmissionMaps[%u]", emissionCount);
                ++emissionCount;
                break;
            default:
                std::cerr << "Invalid texture type\n";
                std::abort();
            }
            shader.UniformI(uniformName, i);
            texture.Texture->Bind(i);
        }
        shader.UniformU("u_Material.DiffuseCount", diffuseCount);
        shader.UniformU("u_Material.SpecularCount", specularCount);
        shader.UniformU("u_Material.EmissionCount", emissionCount);
        m_Vertices.Draw(Cm::DrawMode::Triangles);
    }

    void Mesh::SetTexture(MeshTexture texture)
    {
        m_Textures.push_back(texture);
    }

    Model::Model(const std::string_view& path)
    {
        LoadModel(path);
    }

    Model::Model(Mesh&& mesh)
    {
        AddMesh(std::move(mesh));
    }

    void Model::Draw(Shader& shader) const
    {
        for (const Mesh& mesh : m_Meshes)
            mesh.Draw(shader);
    }

    void Model::AddMesh(Mesh&& mesh)
    {
        m_Meshes.push_back(std::move(mesh));
    }

    void Model::SetAllMeshesTextures(MeshTexture texture)
    {
        for (Mesh& mesh : m_Meshes)
            mesh.SetTexture(texture);
    }

    void Model::LoadModel(const std::string_view& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenUVCoords);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
            std::abort();
        }
        std::string directory(path.substr(0, path.find_last_of('/') + 1));
        ProcessNode(directory, scene->mRootNode, scene);
    }

    void Model::ProcessNode(const std::string& directory, const aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(directory, mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            ProcessNode(directory, node->mChildren[i], scene);
    }

    Mesh Model::ProcessMesh(const std::string& directory, const aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);
        // Vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            vertices.push_back(Vertex{
                glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
                glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
                glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y),
            });
        }
        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }
        // Load Material
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<MeshTexture> meshTextures;
        LoadMaterialTextures(meshTextures, directory, material, aiTextureType_DIFFUSE);
        LoadMaterialTextures(meshTextures, directory, material, aiTextureType_SPECULAR);
        return Mesh(vertices, indices, meshTextures);
    }

    void Model::LoadMaterialTextures(std::vector<MeshTexture>& meshTextures, const std::string& directory, aiMaterial* material, aiTextureType type)
    {
        unsigned int count = material->GetTextureCount(type);
        for (unsigned int i = 0; i < count; ++i)
        {
            aiString str;
            material->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < m_LoadedTextures.size(); ++j)
            {
                // Texture already loaded
                if (strncmp(str.C_Str(), m_LoadedTextures[j].Name.c_str(), str.length) == 0)
                {
                    meshTextures.push_back(MeshTexture{&m_LoadedTextures[i].Texture, type});
                    skip = true;
                    break;
                }
            }
            // Texture isn't already loaded
            if (!skip)
            {
                m_LoadedTextures.push_back(LoadedTexture{Texture(directory + str.C_Str(), TextureFilter::Linear), str.C_Str()});
                meshTextures.push_back(MeshTexture{&m_LoadedTextures.back().Texture, type});
            }
        }
    }
}
