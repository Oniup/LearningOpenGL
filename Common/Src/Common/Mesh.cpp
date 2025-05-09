#include "Common/Mesh.h"

namespace Cm
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<TextureEntry>& textures, VertexBuffer::Type vertexDataType)
        : m_VertexBuffer(vertexDataType), m_Textures(textures)
    {
        m_VertexBuffer.PushData(vertices.size(), vertices.data());
        m_VertexBuffer.PushData(indices.size(), indices.data());
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Draw(Shader& shader)
    {
        size_t diffuseCount = 1;
        size_t specularCount = 1;
        size_t emissionCount = 1;
        for (size_t i = 0; i < m_Textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            constexpr size_t nameBufMaxCount = 128;
            char locationName[nameBufMaxCount];

            auto&[texture, textureType] = m_Textures[i];
            switch (textureType)
            {
            case TextureType::Phong_Diffuse:
                snprintf(locationName, nameBufMaxCount, "u_Material.Diffuse[%zu]", diffuseCount++);
                break;
            case TextureType::Phong_Specular:
                snprintf(locationName, nameBufMaxCount, "u_Material.Specular[%zu]", specularCount++);
                break;
            case TextureType::Phong_Emission:
                snprintf(locationName, nameBufMaxCount, "u_Material.Emission[%zu]", emissionCount++);
                break;
            }

            shader.UniformI(locationName, i);
            glBindTexture(GL_TEXTURE_2D, texture->GetGpuId());
        }
        shader.UniformI("u_Material.DiffuseMapCount", diffuseCount);
        shader.UniformI("u_Material.SpecularMapCount", specularCount);
        shader.UniformI("u_Material.EmissionMapCount", emissionCount);
        m_VertexBuffer.Draw(Cm::DrawMode::Triangles);
    }

    Model::Model(const std::string_view& path)
    {
        LoadModel(path);
    }

    Model::~Model()
    {
    }

    void Model::Draw(Shader& shader)
    {
        for (Mesh& mesh : m_Meshes)
            mesh.Draw(shader);
    }

    void Model::LoadModel(const std::string_view& path)
    {
        Assimp::Importer importer;
        int processSteps = aiProcess_Triangulate | aiProcess_FlipUVs;
        const aiScene* scene = importer.ReadFile(path.data(), processSteps);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
            std::abort();
        }
        std::string directory(path.substr(0, path.find_last_of('/')));
        ProcessNode(directory, scene->mRootNode, scene);
    }

    void Model::ProcessNode(const std::string& directory, const aiNode* node, const aiScene* scene)
    {
        // process all the nodes meshes (if there are any)
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(directory, mesh, scene);
        }
        // process all the other nodes children nodes
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            ProcessNode(directory, node->mChildren[i], scene);
    }

    void Model::ProcessMesh(const std::string& directory, const aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices(mesh->mNumVertices);
        std::vector<unsigned int> indices(mesh->mNumFaces);

        // Vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if (mesh->mTextureCoords[0])
                vertex.UV = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                vertex.UV = glm::vec3(0.0f);
            vertices.push_back(vertex);
        }
        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }
        // Material
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Mesh::TextureEntry> meshTextures;
        LoadMaterialTextures(directory, material, aiTextureType_DIFFUSE, meshTextures);
        LoadMaterialTextures(directory, material, aiTextureType_SPECULAR, meshTextures);

        // Create mesh
        m_Meshes.push_back(Mesh(vertices, indices, meshTextures));
    }

    void Model::LoadMaterialTextures(const std::string& directory, const aiMaterial* mat, aiTextureType type,  std::vector<Mesh::TextureEntry>& out)
    {
        Mesh::TextureType texType;
        switch (type)
        {
        case aiTextureType_DIFFUSE:
            texType = Mesh::TextureType::Phong_Diffuse;
            break;
        case aiTextureType_SPECULAR:
            texType = Mesh::TextureType::Phong_Specular;
            break;
        case aiTextureType_EMISSIVE:
            texType = Mesh::TextureType::Phong_Emission;
            break;
        default:
            std::cerr << "Not supported texture type";
            std::abort();
        }
        for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            Texture* texture = nullptr;
            for (LoadedTexture& loaded : m_LoadedTextures)
            {
                if (strncmp(loaded.Name.data(), str.C_Str(), loaded.Name.size()) == 0)
                {
                    texture = &loaded.Texture;
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                constexpr size_t pathMaxCount = 128;
                char path[pathMaxCount];
                snprintf(path, pathMaxCount, "%s/%s", directory.c_str(), str.C_Str());
                m_LoadedTextures.push_back(LoadedTexture{
                    Texture(path, TextureFilter::Linear),
                    str.C_Str(),
                });
                texture = &m_LoadedTextures.back().Texture;
            }
            out.push_back(Mesh::TextureEntry{texture, texType});
        }
    }
}
