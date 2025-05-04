#pragma once

#include "Common/Mesh.h"

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

        void Bind();
        void Unbind();

        void PushData(size_t vertexCount, const Vertex* vertices);
        void PushData(size_t elementCount, const unsigned int* elements);

        void PushData(size_t offset, size_t vertexCount, const Vertex* vertices);
        void PushData(size_t offset, size_t elementCount, const unsigned int* elements);

        void Draw(DrawMode mode);

    private:
        Type m_Type;
        unsigned int m_Arrays;
        unsigned int m_Vertices;
        unsigned int m_Elements;
        unsigned int m_DataCount;
    };
}
