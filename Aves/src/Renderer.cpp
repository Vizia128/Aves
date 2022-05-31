#include "Renderer.hpp"

namespace Aves {

    Renderer::Renderer(const char* vertexPath, const char* fragmentPath)
    {
        Shader vertexShader;
        vertexShader.Init(vertexPath, "VERTEX");
        Shader fragmentShader;
        fragmentShader.Init(fragmentPath, "FRAGMENT");

        renderShader.Init(vertexShader, fragmentShader);


    }

    Renderer::~Renderer()
    {

    }


}