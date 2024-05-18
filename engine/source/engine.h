#pragma once

#include "d3d.h"
#include "shader_manager.h"

class Engine
{
public:
    static void Init()
    {
        Direct3D::Init();
        ShaderManager::Init();
    }

    static void Deinit()
    {
        // must be in vice versa order to initialization in Engine::Engine()
        ShaderManager::Deinit();
        Direct3D::Deinit();
    }

private:
    Engine() {}
    ~Engine() {}
};
