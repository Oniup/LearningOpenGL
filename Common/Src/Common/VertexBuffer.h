#pragma once

#include "Common/Vertex.h"

namespace Cm
{
    enum class DrawMode
    {
        Points        = 0x0000,
        Lines         = 0x0001,
        LineLoop      = 0x0002,
        LineStrip     = 0x0003,
        Triangles     = 0x0004,
        TriangleStrip = 0x0005,
        TriangleFan  = 0x0006,
    };

    class VertexBuffer
    {
    public:
        enum Type
        {
            Dynamic,
            Static,
        };

    public:
        VertexBuffer(Type type = Static);
        ~VertexBuffer();

        VertexBuffer(VertexBuffer&& buffer);
        VertexBuffer& operator=(VertexBuffer&& buffer);

        void Bind();
        void Unbind();
        void Destroy();

        void PushData(size_t vertexCount, const Vertex* indices);
        void PushData(size_t indicesCount, const unsigned int* indices);
        void PushData(const std::vector<Vertex>& vertices);
        void PushData(const std::vector<unsigned int>& indices);

        void PushData(size_t offset, size_t vertexCount, const Vertex* vertices);
        void PushData(size_t offset, size_t indicesCount, const unsigned int* indices);

        void Draw(DrawMode mode) const;

    private:
        Type m_Type;
        unsigned int m_Arrays;
        unsigned int m_Vertices;
        unsigned int m_Indices;
        unsigned int m_DataCount;
    };
}
