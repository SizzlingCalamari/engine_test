
#include "ResourceLoader.h"

#include "TextureLoader_JPG.h"
#include "ModelLoader_FBX.h"
#include "ModelLoader_OBJ.h"

uint ResourceLoader::LoadResource(const std::string& path)
{
    auto it = m_resourceIds.find(path);
    if (it != m_resourceIds.end())
    {
        return it->second;
    }

    auto extension = GetPathExtension(path);
    uint id = ++m_resourceIdCounter;
    m_resourceIds.emplace(path, id);
    if (extension == "jpg" || extension == "png")
    {
        m_textures.emplace(id, LoadTextureFromJPG(path.c_str()));
    }
    else if (extension == "fbx")
    {
        m_meshes.emplace(id, LoadMeshFromFBX(path.c_str()));
    }
    else if (extension == "obj")
    {
        m_meshes.emplace(id, LoadMeshFromOBJ(path.c_str()));
    }
    else
    {
        assert(false && "unsupported extension");
    }
    return id;
}

std::string ResourceLoader::GetPathExtension(const std::string& path)
{
    auto index = path.find_last_of('.');
    if (index != std::string::npos)
    {
        return path.substr(index + 1);
    }
    return "";
}
