//
// Created by robsc on 10/31/25.
//

#include "QueueFamily.h"

void QueueFamilyIndices::findQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {

	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	size_t i = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) graphicsFamily = i;
		if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) transferFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (presentSupport) presentFamily = i;
		if (transferComplete()) break;
	}


}

bool QueueFamilyIndices::isComplete() {
	return presentFamily.has_value() && graphicsFamily.has_value();
}

bool QueueFamilyIndices::transferComplete() {
	return presentFamily.has_value() && graphicsFamily.has_value() && transferFamily.has_value();
}
