//
// Created by robsc on 10/19/25.
//

#include "RendererCore.h"


namespace SpRenderer {
    bool RendererCore::shouldClose() const {
        return quitWindow;
    }

    void RendererCore::start(const char* ApplicationName) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Vulkan_LoadLibrary(NULL);
        windowName = std::string(ApplicationName);
        startWindow();
    }

    void RendererCore::stop() {
        terminateWindow();
    }

    void RendererCore::endFrame() {
        endWindowFrame();
    }

    void RendererCore::startWindow() {
        windowExtent.width = 800;
        windowExtent.height = 800;



        window = SDL_CreateWindow(windowName.c_str(),windowExtent.width,windowExtent.height,
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
            );

        gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");


        SDL_ClaimWindowForGPUDevice(gpuDevice, window);
    }

    void RendererCore::endWindowFrame() {
        handleWindowEvent();
        SDL_GPUTexture* swapchainTexture;
        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(gpuDevice);

        SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &windowExtent.width, &windowExtent.height);

        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.clear_color = {ClearColor.r / 255.0f, ClearColor.g / 255.0f, ClearColor.b / 255.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.texture = swapchainTexture;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);
        SDL_EndGPURenderPass(renderPass);

        SDL_SubmitGPUCommandBuffer(commandBuffer);
    }

    void RendererCore::terminateWindow() {
        SDL_DestroyGPUDevice(gpuDevice);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void RendererCore::handleWindowEvent() {
        while (SDL_PollEvent(&SdEvent)) {
            switch (SdEvent.type) {
                case SDL_EVENT_QUIT:
                    quitWindow = true;
                    break;
            }
        }
    }

    void RendererCore::createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = windowName.c_str();
        appInfo.pEngineName = "Sparker-Engine";
        appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);


        //Getting extensions
        uint32 instanceExtensionCount = 0;
        const char* const* instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);

        if (instanceExtensions == NULL) {

           SpConsole::FatalExit("Failed to get SDL Vulkan extensions!", SP_FAILURE);
        }

        uint32 extensionCount = instanceExtensionCount + 1;
        const char** extensions = (const char**)SDL_malloc(instanceExtensionCount * sizeof(const char*));
        extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        SDL_memcpy(&extensions[1], instanceExtensions, extensionCount * sizeof(const char*));
        //end Getting Extensions

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = populateDebugMessenger();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.enabledExtensionCount = extensionCount;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;


        // ReSharper disable once CppLocalVariableMayBeConst
        VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanContext.instance);
        SpConsole::VulkanExitCheck(result, "Created Vulkan instance", "Failed to create instance!", SP_FAILURE);

        SDL_free(extensions);

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


        return createInfo;
    }
} // SpRenderer
