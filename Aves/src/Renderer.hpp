#pragma once

#include "Shader.hpp"

namespace Aves {
    class Renderer {

    public:
        Renderer() {}
        Renderer(const char* vertexPath, const char* fragmentPath);
        ~Renderer();


    private:
        Shader renderShader;
    };
}