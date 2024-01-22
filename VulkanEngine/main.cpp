#include "pch.h"
#include "Renderer.h"

int main() 
{
    Renderer renderer(800, 600);

    try
    {
        renderer.Initialize();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
