#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

class VertexArray
{
    private:
        unsigned int m_RendererID;

    public:
        VertexArray();
        ~VertexArray();

        void AddBuffer( const VertexBuffer& vb, const VertexBufferLayout& layout);
        void Bind() const;
        void Unbind() const;
};

#endif