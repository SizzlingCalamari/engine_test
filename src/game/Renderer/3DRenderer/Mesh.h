
#pragma once

class Mesh
{
public:
    void LoadVerticies(const void* verticies, ptrdiff_t stride, size_t num_verticies);

    void FreeVerticies();

    size_t GetNumVerticies() const;

    uint GetVertexBufferId() const;

private:
    uint m_vertex_buffer = 0;
    size_t m_num_verticies = 0;
};

inline size_t Mesh::GetNumVerticies() const
{
    return m_num_verticies;
}

inline uint Mesh::GetVertexBufferId() const
{
    return m_vertex_buffer;
}
