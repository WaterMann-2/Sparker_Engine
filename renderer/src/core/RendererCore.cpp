//
// Created by robsc on 10/19/25.
//

#include "RendererCore.h"

#include <format>


namespace SpRenderer {
    bool RendererCore::shouldClose() const {
        return mainWindow.quitWindow;
    }

    void RendererCore::start(const char* ApplicationName) {
        bool sResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SpConsole::sdlErrorCheck(sResult);
        mainWindow.windowName = std::string(ApplicationName);
        startWindow();
        createInstance();
        createSurface();
        getPhysicalDevice();
        createLogicalDevice();
        createSwapchain();
        createImageViews();
        createRenderpass();
        createGraphicsPipeline();


        std::vector<char> fileData = Utils::FileUtils::readTextFile(RENDERER_RESOURCE_DIR "/testText.txt");
        std::string fileString(fileData.begin(), fileData.end());
        Utils::FileUtils::writeTextFile(RENDERER_DATA_DIR "/awesomeGuy.txt", fileData);
    }

    void RendererCore::stop() {

        destroyRenderpass();
        destroyImageviews();
        destroySwapchain();
        destroyLogicalDevice();
        destroySurface();
        destroyInstance();
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
    }

    void RendererCore::endWindowFrame() {
        handleWindowEvent();
    }

    void RendererCore::terminateWindow() {
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


        std::vector<const char*> extensions = RequiredExtensions;


        //Getting extensions
        {
            uint32 instanceExtensionCount = 0;
            const char* const* instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);
            for (size_t i = 0; i < instanceExtensionCount; i++) {
                extensions.push_back(instanceExtensions[i]);
            }
        }
        //end Getting Extensions

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = populateDebugMessenger();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32>(ValidationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
        createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
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
            SpConsole::sdlErrorCheck(false);
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

        vkGetPhysicalDeviceMemoryProperties(highestDevice->device, &highestDevice->memoryProperties);
        mPhysicalDeviceInfo = *highestDevice;
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

        std::set<std::string> requestedExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

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

    void RendererCore::createLogicalDevice() {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

        std::set<uint32> uniqueQueueFamilies;

        bool transferComplete = mPhysicalDeviceInfo.indices.transferComplete();
        if (transferComplete) {
            uniqueQueueFamilies = {
                             mPhysicalDeviceInfo.indices.graphicsFamily.value(),
                             mPhysicalDeviceInfo.indices.presentFamily.value(),
                             mPhysicalDeviceInfo.indices.transferFamily.value()
            };
        }else {
            uniqueQueueFamilies = {
                         mPhysicalDeviceInfo.indices.graphicsFamily.value(),
                         mPhysicalDeviceInfo.indices.presentFamily.value()
            };
        }

        float queuePriority = 1.0f;
        for (uint32 queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = nullptr;

        deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(DeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

        deviceCreateInfo.enabledLayerCount = static_cast<uint32>(ValidationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();

        VkResult result = vkCreateDevice(mPhysicalDeviceInfo.device, &deviceCreateInfo, nullptr, &mLogicalDevice.device);

        SpConsole::VulkanExitCheck(result, SP_MESSAGE_INFO, "Created logical device", "Failed to create logical device", SP_FAILURE);

        if (transferComplete) {
            vkGetDeviceQueue(mLogicalDevice.device, mPhysicalDeviceInfo.indices.graphicsFamily.value(), 0, &mLogicalDevice.graphicsQueue);
            vkGetDeviceQueue(mLogicalDevice.device, mPhysicalDeviceInfo.indices.presentFamily.value(), 0, &mLogicalDevice.presentQueue);
            vkGetDeviceQueue(mLogicalDevice.device, mPhysicalDeviceInfo.indices.transferFamily.value(), 0, &mLogicalDevice.transferQueue);
        }else {
            vkGetDeviceQueue(mLogicalDevice.device, mPhysicalDeviceInfo.indices.graphicsFamily.value(), 0, &mLogicalDevice.graphicsQueue);
            vkGetDeviceQueue(mLogicalDevice.device, mPhysicalDeviceInfo.indices.presentFamily.value(), 0, &mLogicalDevice.presentQueue);
            mLogicalDevice.transferQueue = nullptr;
        }
    }

    void RendererCore::createSwapchain() {
        mSwapchain.swapchainDetails = &mPhysicalDeviceInfo.swapchainDetails;

        for (const VkSurfaceFormatKHR& swapchainFormat : mPhysicalDeviceInfo.swapchainDetails.formats) {
            if (swapchainFormat.format == VK_FORMAT_B8G8R8A8_UNORM && swapchainFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                mSwapchain.surfaceFormat = swapchainFormat;
                break;
            }
        }

        for (const VkPresentModeKHR& presentMode : mPhysicalDeviceInfo.swapchainDetails.presentModes) {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                mSwapchain.presentMode = presentMode;
                break;
            }
        }

        if (mSwapchain.presentMode != VK_PRESENT_MODE_MAILBOX_KHR) {
            mSwapchain.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        //Check if Extent2D is valid
        if (mSwapchain.swapchainDetails->capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            int width, height;
            SDL_GetWindowSizeInPixels(mainWindow.window, &width, &height);

            VkExtent2D swapchainExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            swapchainExtent.width = std::clamp(swapchainExtent.width,
                                               mSwapchain.swapchainDetails->capabilities.minImageExtent.width,
                                               mSwapchain.swapchainDetails->capabilities.maxImageExtent.width);
            swapchainExtent.height = std::clamp(swapchainExtent.height,
                                                mSwapchain.swapchainDetails->capabilities.minImageExtent.height,
                                                mSwapchain.swapchainDetails->capabilities.maxImageExtent.height);

            mainWindow.extent.width = swapchainExtent.width;
        }

        uint32 imageCount = mSwapchain.swapchainDetails->capabilities.minImageCount + 1;

        if (mSwapchain.swapchainDetails->capabilities.maxImageCount > 0 && imageCount > mSwapchain.swapchainDetails->capabilities.maxImageCount) {
            imageCount = mSwapchain.swapchainDetails->capabilities.maxImageCount;
        }

        std::vector<uint32> queueFamilyIndices;
        bool transferComplete = mPhysicalDeviceInfo.indices.transferComplete();
        if (transferComplete) {
            queueFamilyIndices = {
                mPhysicalDeviceInfo.indices.graphicsFamily.value(),
                mPhysicalDeviceInfo.indices.presentFamily.value(),
                mPhysicalDeviceInfo.indices.transferFamily.value()
            };
        }else {
            queueFamilyIndices = {
                mPhysicalDeviceInfo.indices.graphicsFamily.value(),
                mPhysicalDeviceInfo.indices.presentFamily.value()
            };
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = mainWindow.surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = mSwapchain.surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = mSwapchain.surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = mainWindow.extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (mPhysicalDeviceInfo.indices.graphicsFamily.value() != mPhysicalDeviceInfo.indices.presentFamily.value()) {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32>(queueFamilyIndices.size());
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
        }else {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapchainCreateInfo.preTransform = mSwapchain.swapchainDetails->capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = mSwapchain.presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(mLogicalDevice.device, &swapchainCreateInfo, nullptr, &mSwapchain.swapchain);

        SpConsole::VulkanExitCheck(result, SP_MESSAGE_INFO, "Created swapchain", "Failed to create swapchain!", SP_FAILURE);

        vkGetSwapchainImagesKHR(mLogicalDevice.device, mSwapchain.swapchain, &imageCount, nullptr);
        mSwapchain.images.resize(imageCount);
        vkGetSwapchainImagesKHR(mLogicalDevice.device, mSwapchain.swapchain, &imageCount, mSwapchain.images.data());
    }

    void RendererCore::createImageViews() {
        mSwapchain.imageViews.resize(mSwapchain.images.size());

        for (size_t i = 0; i < mSwapchain.images.size(); i++) {
            VkImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = mSwapchain.images[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = mSwapchain.surfaceFormat.format;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(mLogicalDevice.device, &imageViewCreateInfo, nullptr, &mSwapchain.imageViews[i]);

            SpConsole::VulkanExitCheck(result, SP_MESSAGE_VERBOSE, ("Created imageview: " + std::to_string(i)).c_str(),
                                       "Failed to create imageview!", SP_FAILURE);
        }
    }

    void RendererCore::createRenderpass() {
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        //-------------------//

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = mSwapchain.surfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        //-------------------//

        mDepthResources.format = findDepthFormat();

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = mDepthResources.format;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        //-------------------//

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        //-------------------//
        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachments.size());
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(mLogicalDevice.device, &renderPassCreateInfo, nullptr, &mRenderpass.renderPass);

        SpConsole::VulkanExitCheck(result, SP_MESSAGE_INFO, "Created Renderpass", "Failed to create Renderpass!", SP_FAILURE);

    }

    void RendererCore::createDescriptorSetLayout() {
    }

    void RendererCore::createGraphicsPipeline() {
        m2DMainShader.createShader(
            std::string(RENDERER_RESOURCE_DIR "/shader/Vertex2D Base.vert"),
            std::string(RENDERER_RESOURCE_DIR "/shader/Vertex2D Base.frag"),
            mLogicalDevice.device);
    }

    void RendererCore::createDepthResources() {
        createImage(
            mDepthResources.image,
            mDepthResources.imageMemory,
            mainWindow.extent.width,
            mainWindow.extent.height,
            mDepthResources.format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );

        createImageView(mDepthResources.imageView, mDepthResources.image, mDepthResources.format, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void RendererCore::destroySurface() {
        SDL_Vulkan_DestroySurface(vulkanContext.instance, mainWindow.surface, nullptr);
        SpConsole::Write(SP_MESSAGE_INFO, "Destroyed SDL surface");
    }

    void RendererCore::destroyInstance() {
        vkDestroyInstance(vulkanContext.instance, nullptr);
        SpConsole::Write(SP_MESSAGE_INFO, "Destroyed Vulkan instance");
    }

    void RendererCore::destroyLogicalDevice() {
        vkDestroyDevice(mLogicalDevice.device, nullptr);
        SpConsole::Write(SP_MESSAGE_INFO, "Destroyed Logical device");
    }

    void RendererCore::destroySwapchain() {
        vkDestroySwapchainKHR(mLogicalDevice.device, mSwapchain.swapchain, nullptr);
        SpConsole::Write(SP_MESSAGE_INFO, "Destroyed swapchain");
    }

    void RendererCore::destroyImageviews() {
        for (size_t i = 0; i < mSwapchain.imageViews.size(); i++) {
            vkDestroyImageView(mLogicalDevice.device, mSwapchain.imageViews[i], nullptr);
            SpConsole::Write(SP_MESSAGE_VERBOSE, ("Destroyed image view: " + std::to_string(i)).c_str());
        }
        SpConsole::Write(SP_MESSAGE_INFO, "Destroyed image views");
    }

    void RendererCore::destroyRenderpass() {
        vkDestroyRenderPass(mLogicalDevice.device, mRenderpass.renderPass, nullptr);
        SpConsole::Write(SP_MESSAGE_INFO, "Destroyed render pass");
    }

    VkFormat RendererCore::findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(mPhysicalDeviceInfo.device, format, &props);
            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        SpConsole::FatalExit("Failed to find supported format!", SP_FAILURE);
    }

    VkFormat RendererCore::findDepthFormat() {
        return findSupportedFormat( { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    uint32 RendererCore::findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
        for (size_t i = 0; i < mPhysicalDeviceInfo.memoryProperties.memoryTypeCount; i++) {
            if (typeFilter & (1 << i) && (mPhysicalDeviceInfo.memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
    }

    void RendererCore::createImage(VkImage& image,
                                   VkDeviceMemory& imageMemory,
                                   uint32 width,
                                   uint32 height,
                                   VkFormat format,
                                   VkImageTiling tiling,
                                   VkImageUsageFlags usage,
                                   VkMemoryPropertyFlags
                                   properties) {

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = tiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = usage;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;


        VkResult result = vkCreateImage(mLogicalDevice.device, &imageCreateInfo, nullptr, &image);
        SpConsole::VulkanResult(result, SP_MESSAGE_ERROR, SP_MESSAGE_INFO, "Failed to create image!", "Created image");

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(mLogicalDevice.device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        result = vkAllocateMemory(mLogicalDevice.device, &allocInfo, nullptr, &imageMemory);
        SpConsole::VulkanExitCheck(result, "Failed to allocate memory!", "Allocated memory", SP_FAILURE);
        vkBindImageMemory(mLogicalDevice.device, image, imageMemory, 0);
    }

    void RendererCore::createImageView(VkImageView& imageView,
                                       VkImage image,
                                       VkFormat format,
                                       VkImageAspectFlags aspectFlags) {

        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = format;
        viewCreateInfo.subresourceRange. aspectMask = aspectFlags;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(mLogicalDevice.device, &viewCreateInfo, nullptr, &imageView);
        SpConsole::VulkanExitCheck(result, "Failed to create image view!", "Created image view", SP_FAILURE);
    }
} // SpRenderer
