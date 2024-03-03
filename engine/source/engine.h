#pragma once

#include "d3d.h"
#include "mesh_system.h"

class Engine
{
public:
    static void Init()
    {
        Direct3D::Init();
        MeshSystem::Init();
    }

    static void Deinit()
    {
        // must be in vice versa order to initialization in Engine::Engine()
        MeshSystem::Deinit();
        Direct3D::Deinit();
    }

private:
    Engine() {}
    ~Engine() {}
};
