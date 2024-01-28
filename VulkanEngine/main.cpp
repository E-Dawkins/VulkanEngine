#include "pch.h"

#include "Renderer.h"
#include "TestGameObject.h"

int main() 
{
    Renderer::GetInstance()->Initialize(800, 600);
    
    auto* testObject = new TestGameObject();
    testObject->BeginPlay();
    
    auto mainThreadFunc = [&]()
    {
        auto lastTime = high_resolution_clock::now();
        
        while (Renderer::GetInstance()->IsRunning())
        {
            try
            {
                const auto currentTime = high_resolution_clock::now();
                const float deltaTime = duration<float>(currentTime - lastTime).count();

                // TODO - remove rudimentary fps cap
                constexpr int fps = 60;
                if (deltaTime < 1.f / static_cast<float>(fps))
                {
                    continue;
                }
                
                glfwPollEvents();
                Renderer::GetInstance()->DrawFrame();
                
                testObject->Tick(deltaTime);

                lastTime = currentTime;
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        vkDeviceWaitIdle(Renderer::GetInstance()->GetDevice());

        testObject->EndPlay();
        Renderer::GetInstance()->Cleanup();
    };

    // Initialize threads
    std::thread mainThread(mainThreadFunc);

    // Wait for threads to finish
    mainThread.join();

    return EXIT_SUCCESS;
}
