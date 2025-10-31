//
// Created by robsc on 10/19/25.
//

#ifndef SPARKER_ENGINE_RENDERERCORE_H
#define SPARKER_ENGINE_RENDERERCORE_H


#include "Utils/Utils.h"


const uvec3 ClearColor = uvec3(25, 40, 60);

namespace SpRenderer {
    class RendererCore {
    public:
        bool shouldClose() const;

        void start(const char* ApplicationName);
        void stop();

        void endFrame();

    private:
        struct VulkanContext {
            VkInstance instance;
            VkDebugUtilsMessengerEXT debugMessenger;
        };

    private:
        SDL_Window* window;
        SDL_GPUDevice* gpuDevice;
        SDL_Renderer* Sdrenderer;
        SDL_Event SdEvent;
        bool quitWindow = false;

        std::string windowName;
        VkExtent2D windowExtent;

        VulkanContext vulkanContext;

    private:
        void startWindow();
        void endWindowFrame();
        void terminateWindow();
        void handleWindowEvent();

        void createInstance();
        static VkDebugUtilsMessengerCreateInfoEXT populateDebugMessenger();
    };
} // SpRenderer

#endif //SPARKER_ENGINE_RENDERERCORE_H
