//
// Created by robsc on 10/19/25.
//

#ifndef SPARKER_ENGINE_RENDERERCORE_H
#define SPARKER_ENGINE_RENDERERCORE_H


#include "QueueFamily.h"
#include "Utils/Utils.h"


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
			SDL_Event SdEvent;
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

		struct VulkanContext {
			VkInstance instance;
			VkDebugUtilsMessengerEXT debugMessenger;
			PhysicalDeviceInfo physicalDeviceInfo;

		};

#pragma endregion PrivateStructs

	private:
		SdlContext mainWindow;
		VulkanContext vulkanContext;

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

		void inline terminateSurface();
		void inline terminateInstance();

	private:

	};
} // SpRenderer

#endif //SPARKER_ENGINE_RENDERERCORE_H
