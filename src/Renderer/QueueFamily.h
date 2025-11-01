//
// Created by robsc on 10/31/25.
//

#ifndef SPARKER_ENGINE_QUEUEFAMILY_H
#define SPARKER_ENGINE_QUEUEFAMILY_H

#include "Utils/Utils.h"
#include <optional>


struct QueueFamilyIndices {
	std::optional<int> graphicsFamily;
	std::optional<int> presentFamily;
	std::optional<int> transferFamily;

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