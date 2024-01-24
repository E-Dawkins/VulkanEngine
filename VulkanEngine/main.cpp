#include "pch.h"
#include "Renderer.h"

int main() 
{
    try
    {
        Renderer::GetInstance()->Initialize(800, 600);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
