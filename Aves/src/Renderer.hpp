#pragma once

#include "Shader.hpp"

namespace Aves {
    class Renderer {

    public:
        Renderer();
        ~Renderer();

    private:
        Shader renderShader;
        unsigned int numVertices;
        unsigned int VBO, VAO;
    };
}