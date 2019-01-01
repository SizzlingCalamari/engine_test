
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

void ParseFBX
(
    const char *filename,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvcoords,
    std::vector<glm::vec3>& normals,
    glm::vec3& minAABB,
    glm::vec3& maxAABB
)
{
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

        const fbxsdk::FbxMesh *nodeMesh = static_cast<const fbxsdk::FbxMesh*>(nodeAttribute);

        const_cast<fbxsdk::FbxMesh*>(nodeMesh)->ComputeBBox();
        const fbxsdk::FbxDouble3 bboxMin = nodeMesh->BBoxMin;
        const fbxsdk::FbxDouble3 bboxMax = nodeMesh->BBoxMax;

        minAABB = glm::vec3(bboxMin.mData[0], bboxMin.mData[1], bboxMin.mData[2]);
        maxAABB = glm::vec3(bboxMax.mData[0], bboxMax.mData[1], bboxMax.mData[2]);

        // Vertices
        {
            fbxsdk::FbxVector4* controlPoints = nodeMesh->GetControlPoints();
            const int* indices = nodeMesh->GetPolygonVertices();
            const int numIndices = nodeMesh->GetPolygonVertexCount();
            assert((numIndices % 3) == 0);
            if (numIndices > 0)
            {
                vertices.resize(numIndices);
                glm::vec3* verticesOut = &vertices[0];
                for (int j = 0; j < numIndices; ++j)
                {
                    const fbxsdk::FbxVector4& vertex = controlPoints[indices[j]];
                    verticesOut[j] = glm::vec3(vertex.mData[0], vertex.mData[1], vertex.mData[2]);
                }
            }
        }

        // Normals
        fbxsdk::FbxArray<fbxsdk::FbxVector4> normalsArray;
        if (nodeMesh->GetPolygonVertexNormals(normalsArray))
        {
            const fbxsdk::FbxVector4* fbxNormals = normalsArray.GetArray();
            const int numNormals = normalsArray.Size();
            if (numNormals > 0)
            {
                normals.resize(numNormals);
                glm::vec3* normalsOut = &normals[0];
                for (int j = 0; j < numNormals; ++j)
                {
                    const fbxsdk::FbxVector4& normal = fbxNormals[j];
                    normalsOut[j] = glm::vec3(normal.mData[0], normal.mData[1], normal.mData[2]);
                }
            }
        }

        // UVs

        break;
    }
    DestroyFbxObjects(fbxManager);
}

Mesh LoadMeshFromFBX(const char *filename)
{
    Mesh mesh;
    ParseFBX(filename, mesh.vertices, mesh.uvcoords, mesh.normals, mesh.minAABB, mesh.maxAABB);
    return mesh;
}
