//
// Created by robsc on 10/31/25.
//

#ifndef SPARKER_ENGINE_QUEUEFAMILY_H
#define SPARKER_ENGINE_QUEUEFAMILY_H

#include "Utils/Utils.h"
#include <optional>


struct QueueFamilyIndices {
	std::optional<uint32> graphicsFamily;
	std::optional<uint32> presentFamily;
	std::optional<uint32> transferFamily;

	void findQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	bool isComplete();
	bool transferComplete();
};

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	bool compatiable() {
		return !formats.empty() && !presentModes.empty();
	}
};

#endif //SPARKER_ENGINE_QUEUEFAMILY_H