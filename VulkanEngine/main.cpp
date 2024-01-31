#include "pch.h"

#include "GravityGameObject.h"
#include "Material_Base.h"
#include "Material_Unlit.h"
#include "Renderer.h"
#include "PhysicsSolver.h"
#include "TestGameObject.h"

int main() 
{
    Renderer::GetInstance()->Initialize(800, 600);

    // Load Meshes
    Renderer::GetInstance()->LoadMesh("models/viking_room.obj", "house", true);

    // Load Textures
    Renderer::GetInstance()->LoadTexture("textures/viking_room.png", "house");

    // Load Materials
    const auto material = Renderer::GetInstance()->LoadMaterial<Material_Unlit>("shaders/unlit.vert.spv", "shaders/unlit.frag.spv", "house");
    material->SetTexture(Renderer::GetInstance()->GetTexture("house"));
    material->Init();

    // Initialize Game Objects
    int rows = 100, cols = 100;

    std::vector<GameObject*> gameObjects;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            gameObjects.push_back(new TestGameObject());
            gameObjects.back()->GetRoot()->transform.SetWorldPosition(glm::vec3(i - (rows/2), j - (cols/2), 0));
        }
    }

    gameObjects.push_back(new GravityGameObject());
    gameObjects.back()->GetRoot()->transform.SetWorldPosition(glm::vec3(-1.1f, -0.8f, 1.5f));
    
    for (const auto object : gameObjects)
    {
        object->BeginPlay();
    }
    
    auto lastTime = high_resolution_clock::now();

    // Start the physics thread
    std::thread physicsThread([]
    {
        PhysicsSolver::GetInstance()->Init();
    });
        
    while (Renderer::GetInstance()->IsRunning())
    {
        try
        {
            const auto currentTime = high_resolution_clock::now();
            const float deltaTime = duration<float>(currentTime - lastTime).count();

            g_timeSinceAppStart += deltaTime;
                
            // Game object ticks happen first
            for (const auto object : gameObjects)
            {
                object->Tick(deltaTime);
            }

            // Drawing will happen at the end of the frame
            glfwPollEvents();
            Renderer::GetInstance()->DrawFrame();

            lastTime = currentTime;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    // Wait for physics thread to finish
    physicsThread.join();

    // Wait for renderer to finish with this frame
    vkDeviceWaitIdle(Renderer::GetInstance()->GetDevice());

    // Cleanup all game objects (including materials and their graphics resources)
    for (const auto object : gameObjects)
    {
        object->EndPlay();
    }

    // Finally cleanup the renderer
    Renderer::GetInstance()->Cleanup();

    return EXIT_SUCCESS;
}
