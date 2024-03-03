#include "mesh_system.h"

MeshSystem *MeshSystem::s_instance = nullptr;

void MeshSystem::Init()
{
    if (!s_instance)
        s_instance = new MeshSystem;
    else
        std::cout << "MeshSystem::Init() was called twice!\n";
}

MeshSystem *MeshSystem::GetInstance()
{
    assert(s_instance && "MeshSystem is not initialized!");

    return s_instance;
}

void MeshSystem::Deinit()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
    else
        std::cout << "MeshSystem::Deinit() was called before MeshSystem::Init() or twice!\n";
}

void MeshSystem::SetMesh(Vertex *vertices, uint64_t byteSize)
{
    m_vertexBuffer.Init(vertices, byteSize);
}
