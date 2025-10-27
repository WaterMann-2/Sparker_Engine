//
// Created by robsc on 10/19/25.
//

#ifndef SPARKER_ENGINE_RENDERERCORE_H
#define SPARKER_ENGINE_RENDERERCORE_H

#include "../Renderer/Utils/utils.h"

namespace SpRenderer {
    class RendererCore {
    public:
        bool shouldClose() const;

        void start(const char* ApplicationName);
        void stop();

        void endFrame();

    private:
        GLFWwindow* window;
        std::string windowName;
        VkExtent2D windowExtent;

    private:
        void startWindow();
        void endWindowFrame();
        void terminateWindow();


        void createInstance();
        static std::vector<const char*> getRequiredExtensions();
        static VkDebugUtilsMessengerCreateInfoEXT populateDebugMessenger();
    };
} // SpRenderer

#endif //SPARKER_ENGINE_RENDERERCORE_H
