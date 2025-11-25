//
// Created by robsc on 10/19/25.
//

#ifndef SPARKER_ENGINE_RENDERERCORE_H
#define SPARKER_ENGINE_RENDERERCORE_H


#include "QueueFamily.h"
#include "Utils.h"
#include "Shader.h"


const uvec3 ClearColor = uvec3(25, 40, 60);

namespace SpRenderer {
	// ReSharper disable once CppClassNeedsConstructorBecauseOfUninitializedMember
	class RendererCore {
	public:
		bool shouldClose() const;

		void start(const char* ApplicationName);
		void stop();

		void endFrame();

	private:
#pragma region PrivateStructs
		struct SdlContext {
			SDL_Window* window;
			bool quitWindow = false;
			std::string windowName;
			VkExtent2D extent;
			VkSurfaceKHR surface;
		};

		struct PhysicalDeviceInfo {
			VkPhysicalDevice device;
			SwapchainSupportDetails swapchainDetails;
			QueueFamilyIndices indices;
			int32 score;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;
		};

		struct LogicalDevice {
			VkDevice device;

			VkQueue graphicsQueue;
			VkQueue presentQueue;
			VkQueue transferQueue;
		};

		struct Swapchain {
			VkSwapchainKHR swapchain;
			SwapchainSupportDetails* swapchainDetails;
			VkSurfaceFormatKHR surfaceFormat;
			VkPresentModeKHR presentMode;

			std::vector<VkImage> images = std::vector<VkImage>(0);
			std::vector<VkImageView> imageViews = std::vector<VkImageView>(0);
		};

		struct Renderpass {
			VkRenderPass renderPass;


		};

		struct VulkanContext {
			VkInstance instance;
			VkDebugUtilsMessengerEXT debugMessenger;
		};

		struct DepthResources {
			VkFormat format;
			VkImage image;
			VkDeviceMemory imageMemory;
			VkImageView imageView;
		};

#pragma endregion PrivateStructs

	private:
		SdlContext mainWindow;
		VulkanContext vulkanContext;

		PhysicalDeviceInfo mPhysicalDeviceInfo;
		LogicalDevice mLogicalDevice;
		Swapchain mSwapchain;
		Renderpass mRenderpass;

		DepthResources mDepthResources;

		Shader m2DMainShader;

	private:
		void startWindow();
		void endWindowFrame();
		void terminateWindow();
		void handleWindowEvent();

		void createInstance();
		static VkDebugUtilsMessengerCreateInfoEXT populateDebugMessenger();

		void createSurface();

		void getPhysicalDevice();
		int isSuitableDevice(PhysicalDeviceInfo& deviceInfo);
		void querySwapchainSupport(PhysicalDeviceInfo& deviceInfo);

		void createLogicalDevice();
		void createSwapchain();
		void createImageViews();
		void createRenderpass();
		void createDescriptorSetLayout();
		void createGraphicsPipeline();
		void createDepthResources();
		void createFramebuffers();
		void createCommandPool();
		void createTextureImage();

		void createUniformBuffers();
		void createDescriptorPool();
		void createDescriptorSets();

		void createCommandBuffers();
		void createSyncObjects();




		void inline destroySurface();
		void inline destroyInstance();
		void inline destroyLogicalDevice();
		void inline destroySwapchain();
		void inline destroyImageviews();
		void inline destroyRenderpass();

	private:
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();

		uint32 findMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);

		void createImage(VkImage& image,
		                 VkDeviceMemory& imageMemory,
		                 uint32 width,
		                 uint32 height,
		                 VkFormat format,
		                 VkImageTiling tiling,
		                 VkImageUsageFlags usage,
		                 VkMemoryPropertyFlags
		                 properties);

		void createImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	};
} // SpRenderer

#endif //SPARKER_ENGINE_RENDERERCORE_H
