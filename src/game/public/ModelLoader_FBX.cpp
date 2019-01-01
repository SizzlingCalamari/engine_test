
#include "ModelLoader_FBX.h"

#include <glm/glm.hpp>
#include <vector>
#include <fbxsdk.h>

using namespace std;

bool InitializeFbxObjects(fbxsdk::FbxManager **fbxManager_out, fbxsdk::FbxScene **fbxScene_out)
{
    assert(fbxManager_out);
    assert(fbxScene_out);
    fbxsdk::FbxManager *fbxManager = fbxsdk::FbxManager::Create();
    if (!fbxManager)
    {
        return false;
    }
    
    fbxsdk::FbxIOSettings* fbxIoSettings = fbxsdk::FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(fbxIoSettings);

    fbxsdk::FbxString path = fbxsdk::FbxGetApplicationDirectory();
    fbxManager->LoadPluginsDirectory(path.Buffer());

    fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(fbxManager, "FBX Default Scene");
    if (!fbxScene)
    {
        return false;
    }

    *fbxManager_out = fbxManager;
    *fbxScene_out = fbxScene;
    return true;
}

void DestroyFbxObjects(fbxsdk::FbxManager *fbxManager)
{
    fbxManager->Destroy();
}

bool LoadFbxScene(fbxsdk::FbxManager *fbxManager, fbxsdk::FbxScene *fbxScene, const char *filename)
{
    fbxsdk::FbxImporter *fbxImporter = fbxsdk::FbxImporter::Create(fbxManager, "FBX Default Importer");
    bool importStatus = fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings());
    if (!importStatus)
    {
        return false;
    }

    if (!fbxImporter->IsFBX())
    {
        return false;
    }

    importStatus = fbxImporter->Import(fbxScene);
    fbxImporter->Destroy();
    return importStatus;
}

vector<glm::vec3> ParseFBX(const char *filename)
{
    vector<glm::vec3> vertices;
    fbxsdk::FbxManager *fbxManager = nullptr;
    fbxsdk::FbxScene *fbxScene = nullptr;

    InitializeFbxObjects(&fbxManager, &fbxScene);
    LoadFbxScene(fbxManager, fbxScene, filename);

    fbxsdk::FbxNode* rootNode = fbxScene->GetRootNode();
    const int numChildren = rootNode->GetChildCount();
    for (int i = 0; i < numChildren; ++i)
    {
        const fbxsdk::FbxNode* childNode = rootNode->GetChild(i);
        const fbxsdk::FbxNodeAttribute* nodeAttribute = childNode->GetNodeAttribute();
        if (!nodeAttribute)
        {
            continue;
        }

        const fbxsdk::FbxNodeAttribute::EType attributeType = nodeAttribute->GetAttributeType();
        if (attributeType != fbxsdk::FbxNodeAttribute::eMesh)
        {
            continue;
        }

        auto *nodeMesh = static_cast<FbxMesh*>(nodeAttribute);
        FbxVector4 *nodeVertices = nodeMesh->GetControlPoints();
        for (int j = 0; j < nodeMesh->GetPolygonCount(); ++j)
        {
            int numVertices = nodeMesh->GetPolygonSize(j);
            assert(numVertices == 3);

            for (int k = 0; k < numVertices; ++k)
            {
                int controlPointIndex = nodeMesh->GetPolygonVertex(j, k);

                vertices.emplace_back(nodeVertices[controlPointIndex].mData[0],
                                       nodeVertices[controlPointIndex].mData[1],
                                       nodeVertices[controlPointIndex].mData[2]);
            }
        }
    }
    DestroyFbxObjects(fbxManager);
    return vertices;
}

Mesh LoadMeshFromFBX(const char *filename)
{
    Mesh mesh;
    mesh.vertices = ParseFBX(filename);
    return mesh;
}
