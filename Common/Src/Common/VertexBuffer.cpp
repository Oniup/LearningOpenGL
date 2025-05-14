#include "Common/VertexBuffer.h"

#include <glad/glad.h>

namespace Cm
{
    VertexBuffer::VertexBuffer(Type type)
        : m_Type(type), m_Indices(std::numeric_limits<unsigned int>::max()), m_DataCount(0)
    {
        glGenVertexArrays(1, &m_Arrays);
        glGenBuffers(1, &m_Vertices);
        glBindVertexArray(m_Arrays);
        glBindBuffer(GL_ARRAY_BUFFER, m_Vertices);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Position)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, UV)));

        glBindVertexArray(0);
    }

    VertexBuffer::~VertexBuffer()
    {
        Destroy();
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& buffer)
        : m_Type(buffer.m_Type), m_Arrays(buffer.m_Arrays), m_Vertices(buffer.m_Vertices), m_Indices(buffer.m_Indices), m_DataCount(buffer.m_DataCount)
    {
        buffer.m_Arrays = std::numeric_limits<unsigned int>::max();
        buffer.m_Vertices = std::numeric_limits<unsigned int>::max();
        buffer.m_Indices = std::numeric_limits<unsigned int>::max();
        buffer.m_DataCount = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& buffer)
    {
        Destroy();

        m_Arrays = buffer.m_Arrays;
        m_Vertices = buffer.m_Vertices;
        m_Indices = buffer.m_Indices;
        m_DataCount = buffer.m_DataCount;

        buffer.m_Arrays = std::numeric_limits<unsigned int>::max();
        buffer.m_Vertices = std::numeric_limits<unsigned int>::max();
        buffer.m_Indices = std::numeric_limits<unsigned int>::max();
        buffer.m_DataCount = 0;
        return *this;
    }

    void VertexBuffer::Bind()
    {
        glBindVertexArray(m_Arrays);
    }

    void VertexBuffer::Unbind()
    {
        glBindVertexArray(0);
    }

    void VertexBuffer::Destroy()
    {
        if (m_Arrays != std::numeric_limits<unsigned int>::max())
        {
            glDeleteVertexArrays(1, &m_Arrays);
            glDeleteBuffers(1, &m_Vertices);
            if (m_Indices != std::numeric_limits<unsigned int>::max())
            {
                glDeleteBuffers(1, &m_Indices);
            }
            m_Arrays = std::numeric_limits<unsigned int>::max();
            m_Vertices = std::numeric_limits<unsigned int>::max();
            m_Indices = std::numeric_limits<unsigned int>::max();
            m_DataCount = 0;
        }
    }

    void VertexBuffer::PushData(size_t vertexCount, const Vertex* vertices)
    {
        glBindVertexArray(m_Arrays);
        glBindBuffer(GL_ARRAY_BUFFER, m_Vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexCount, vertices, m_Type == Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        if (m_Indices == std::numeric_limits<unsigned int>::max())
            m_DataCount = vertexCount;
    }

    void VertexBuffer::PushData(size_t indicesCount, const unsigned int* indicess)
    {
        glBindVertexArray(m_Arrays);
        if (m_Indices == std::numeric_limits<unsigned int>::max())
            glGenBuffers(1, &m_Indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, indicess, m_Type == Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        m_DataCount = indicesCount;
    }

    void VertexBuffer::PushData(const std::vector<Vertex>& vertices)
    {
        PushData(vertices.size(), vertices.data());
    }

    void VertexBuffer::PushData(const std::vector<unsigned int>& indices)
    {
        PushData(indices.size(), indices.data());
    }

    void VertexBuffer::PushData(size_t offset, size_t vertexCount, const Vertex* vertices)
    {
        glBindVertexArray(m_Arrays);
        if (m_Vertices != std::numeric_limits<unsigned int>::max())
            PushData(offset + vertexCount, (Vertex*)nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, m_Vertices);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * offset, sizeof(Vertex) * vertexCount, vertices);
    }

    void VertexBuffer::PushData(size_t offset, size_t indicesCount, const unsigned int* indicess)
    {
        glBindVertexArray(m_Arrays);
        if (m_Indices != std::numeric_limits<unsigned int>::max())
            PushData(offset + indicesCount, (unsigned int*)nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * offset, sizeof(unsigned int) * indicesCount, indicess);
    }

    void VertexBuffer::Draw(DrawMode mode) const
    {
        glBindVertexArray(m_Arrays);
        if (m_Indices != std::numeric_limits<unsigned int>::max())
            glDrawElements((GLenum)mode, m_DataCount, GL_UNSIGNED_INT, (void*)0);
        else
            glDrawArrays((GLenum)mode, 0, m_DataCount);
    }
}
