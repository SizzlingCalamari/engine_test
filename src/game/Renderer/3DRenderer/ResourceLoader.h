
#pragma once

#include "Mesh.h"
#include "Texture.h"

#include <unordered_map>
#include <string>

class ResourceLoader
{
public:
    uint LoadResource(const std::string& path);

    void UnloadResources();

    const Mesh* GetMesh(uint id);

    const Texture* GetTexture(uint id);

private:
    static std::string GetPathExtension(const std::string& path);

private:
    std::unordered_map<std::string, uint> m_resourceIds;
    std::unordered_map<uint, Mesh> m_meshes;
    std::unordered_map<uint, Texture> m_textures;
    uint m_resourceIdCounter = 0;
};

inline const Mesh* ResourceLoader::GetMesh(uint id)
{
    assert(m_meshes.find(id) != m_meshes.end());
    return &m_meshes[id];
}

inline const Texture* ResourceLoader::GetTexture(uint id)
{
    assert(m_textures.find(id) != m_textures.end());
    return &m_textures[id];
}

inline void ResourceLoader::UnloadResources()
{
    for (auto it : m_meshes)
    {
        auto &mesh = it.second;
        FreeMesh(&mesh);
    }
    for (auto it : m_textures)
    {
        auto &texture = it.second;
        texture.FreeTexture();
    }
}
