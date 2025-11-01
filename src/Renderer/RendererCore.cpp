//
// Created by robsc on 10/19/25.
//

#include "RendererCore.h"

#include <format>

#include "SDL3/SDL_main_impl.h"


namespace SpRenderer {
    bool RendererCore::shouldClose() const {
        return mainWindow.quitWindow;
    }

    void RendererCore::start(const char* ApplicationName) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Vulkan_LoadLibrary(nullptr);
        mainWindow.windowName = std::string(ApplicationName);
        startWindow();
        createInstance();
        createSurface();
        getPhysicalDevice();
    }

    void RendererCore::stop() {

        terminateSurface();
        terminateInstance();
        terminateWindow();
    }

    void RendererCore::endFrame() {
        endWindowFrame();
    }

    void RendererCore::startWindow() {
        mainWindow.extent.width = 800;
        mainWindow.extent.height = 800;



        mainWindow.window = SDL_CreateWindow(mainWindow.windowName.c_str(),mainWindow.extent.width,mainWindow.extent.height,
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
            );

        mainWindow.gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");


        SDL_ClaimWindowForGPUDevice(mainWindow.gpuDevice, mainWindow.window);
    }

    void RendererCore::endWindowFrame() {
        handleWindowEvent();
    }

    void RendererCore::terminateWindow() {
        SDL_DestroyGPUDevice(mainWindow.gpuDevice);
        SDL_DestroyWindow(mainWindow.window);
        SDL_Quit();
    }

    void RendererCore::handleWindowEvent() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    mainWindow.quitWindow = true;
                    break;
            }
        }
    }

    void RendererCore::createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = mainWindow.windowName.c_str();
        appInfo.pEngineName = "Sparker-Engine";
        appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);


        //Getting extensions
        uint32 instanceExtensionCount = 0;
        const char* const* instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);

        if (instanceExtensions == nullptr) {
            SpConsole::FatalExit("Failed to get SDL Vulkan extensions!", SP_FAILURE);
        }

        uint32 extensionCount = instanceExtensionCount + 1;
        const char** extensions = static_cast<const char**>(SDL_malloc(instanceExtensionCount * sizeof(const char*)));
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

        uint32 validationLayersCount = 0;
        vkEnumerateInstanceLayerProperties(&validationLayersCount, nullptr);
        std::vector<VkLayerProperties> availableValidationLayers(validationLayersCount);
        vkEnumerateInstanceLayerProperties(&validationLayersCount, availableValidationLayers.data());

        for (size_t i = 0; i < availableValidationLayers.size(); i++) {
            SpConsole::Write(SP_MESSAGE_VERBOSE, (std::string("Found layer: ") + availableValidationLayers[i].layerName).c_str());
        }

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
        createInfo.pfnUserCallback = SP_VK_MESSAGE_CALLBACK;
        createInfo.pUserData = nullptr;
        createInfo.pNext = nullptr;


        return createInfo;
    }

    void RendererCore::createSurface() {
        bool result = SDL_Vulkan_CreateSurface(mainWindow.window, vulkanContext.instance, nullptr, &mainWindow.surface);
        if (!result) {
            SpConsole::FatalExit("Failed to create Vulkan surface!", SP_FAILURE);
        }

        SpConsole::Write(SP_MESSAGE_INFO, "Created Vulkan surface");
    }

    void RendererCore::getPhysicalDevice() {
        VkPhysicalDevice physicalDevice = nullptr;
        uint32 deviceCount = 0;
        vkEnumeratePhysicalDevices(vulkanContext.instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            SpConsole::FatalExit("Failed to find GPU with Vulkan support!", SP_FAILURE);
        }else {
            std::string message = "Found " + std::to_string(deviceCount) + " GPUs";
            SpConsole::Write(SP_MESSAGE_INFO, message.c_str());
        }


        std::vector<VkPhysicalDevice> vkDevices(deviceCount);
        vkEnumeratePhysicalDevices(vulkanContext.instance, &deviceCount, vkDevices.data());

        std::vector<PhysicalDeviceInfo> devices(deviceCount);
        for (size_t i = 0; i < deviceCount; i++) {
            devices[i].device = vkDevices[i];
        }

        size_t i = 0;
        int32 highestScore = -1000;
        PhysicalDeviceInfo* highestDevice = nullptr;
        for (PhysicalDeviceInfo& device : devices) {
            if (isSuitableDevice(device) && device.score > highestScore) {
                highestScore = device.score;
                highestDevice = &devices[i];
            }
            i++;
        }

        if (highestDevice == nullptr) {
            SpConsole::FatalExit("Failed to find suitable GPU!", SP_FAILURE);
        }

        vulkanContext.physicalDeviceInfo = *highestDevice;
    }

    int RendererCore::isSuitableDevice(PhysicalDeviceInfo& deviceInfo) {
        if (deviceInfo.device == NULL) {
            return std::numeric_limits<uint32>::min();
        }
        vkGetPhysicalDeviceProperties(deviceInfo.device, &deviceInfo.properties);
        vkGetPhysicalDeviceFeatures(deviceInfo.device, &deviceInfo.features);

        uint32 extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(deviceInfo.device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(deviceInfo.device, nullptr, &extensionCount, extensions.data());

        std::set<std::string> requestedExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const VkExtensionProperties& extension : extensions) {
            requestedExtensions.erase(extension.extensionName);
        }

        bool extensionsFound = requestedExtensions.empty();

        bool swapchainAdequate = false;
        if (extensionsFound) {
            querySwapchainSupport(deviceInfo);
            swapchainAdequate = !deviceInfo.swapchainDetails.formats.empty() && !deviceInfo.swapchainDetails.presentModes.empty();
        }

        deviceInfo.indices.findQueueIndices(deviceInfo.device, mainWindow.surface);

        int validDevice = deviceInfo.indices.isComplete() && extensionsFound && swapchainAdequate ? 1 : 0;
        int dedicatedGraphics = deviceInfo.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1 : 0;


        deviceInfo.score = validDevice + dedicatedGraphics;

        return deviceInfo.score > 0;
    }

    void RendererCore::querySwapchainSupport(PhysicalDeviceInfo& deviceInfo) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(deviceInfo.device, mainWindow.surface, &deviceInfo.swapchainDetails.capabilities);

        uint32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(deviceInfo.device, mainWindow.surface, &formatCount, nullptr);

        if (formatCount != 0) {
            deviceInfo.swapchainDetails.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(deviceInfo.device, mainWindow.surface, &formatCount, deviceInfo.swapchainDetails.formats.data());
        }

        uint32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(deviceInfo.device, mainWindow.surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            deviceInfo.swapchainDetails.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(deviceInfo.device, mainWindow.surface, &presentModeCount, deviceInfo.swapchainDetails.presentModes.data());
        }
    }

    void RendererCore::terminateSurface() {
        SDL_Vulkan_DestroySurface(vulkanContext.instance, mainWindow.surface, nullptr);
    }

    void RendererCore::terminateInstance() {
        vkDestroyInstance(vulkanContext.instance, nullptr);
    }
} // SpRenderer
