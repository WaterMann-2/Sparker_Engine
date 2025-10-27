//
// Created by robsc on 10/19/25.
//

#include "RendererCore.h"


namespace SpRenderer {
    bool RendererCore::shouldClose() const { return glfwWindowShouldClose(window); }

    void RendererCore::start(const char* ApplicationName) {
        glfwInit();
        windowName = std::string(ApplicationName);
        startWindow();
    }

    void RendererCore::stop() { terminateWindow(); }

    void RendererCore::endFrame() { endWindowFrame(); }

    void RendererCore::startWindow() {
        windowExtent.width = 800;
        windowExtent.height = 800;


        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        window = glfwCreateWindow(windowExtent.width, windowExtent.height, windowName.c_str(), NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
    }

    void RendererCore::endWindowFrame() { glfwPollEvents(); }

    void RendererCore::terminateWindow() { glfwTerminate(); }

    void RendererCore::createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = windowName.c_str();
        appInfo.pEngineName = "Sparker-Engine";
        appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        std::vector<const char*> glfwExtensions = getRequiredExtensions();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.enabledExtensionCount = static_cast<uint32>(validationLayers.size());
        createInfo.ppEnabledExtensionNames = glfwExtensions.data();
    }

    std::vector<const char*> RendererCore::getRequiredExtensions() {
        uint32 glfwExtensionCount = 0;

        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if ( enableValidationLayers ) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    VkDebugUtilsMessengerCreateInfoEXT RendererCore::populateDebugMessenger() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    }
} // SpRenderer
