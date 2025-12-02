//
// Created by robsc on 10/19/25.
//

#ifndef SPARKER_ENGINE_UTILS_H
#define SPARKER_ENGINE_UTILS_H
#define DEBUG

#include <cstdint>
#include <iostream>
#include <ostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <set>
#include <limits>
#include <chrono>

#define SDL_EVENT
#define SDL_MAIN_HANDLED
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

#include <SpRendererConfig.h>

#define SP_VK_MESSAGE_CALLBACK SpConsole::vkDebugCallback

#ifdef DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

const std::vector<const char*> ValidationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const std::vector<const char*> RequiredExtensions = {
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME
};

typedef std::int8_t int8;
typedef std::uint8_t uint8;
typedef std::int16_t int16;
typedef std::uint16_t uint16;
typedef std::int32_t int32;
typedef std::uint32_t uint32;
typedef std::int64_t int64;
typedef std::uint64_t uint64;

typedef glm::vec2 vec2;
typedef glm::uvec2 uvec2;
typedef glm::vec3 vec3;
typedef glm::uvec3 uvec3;
typedef glm::vec4 vec4;
typedef glm::uvec4 uvec4;

typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;


enum MessageSeverity {
	SP_MESSAGE_VERBOSE,
	SP_MESSAGE_INFO,
	SP_MESSAGE_WARNING,
	SP_MESSAGE_ERROR,
	SP_MESSAGE_FATAL
};

enum ExitCode {
	SP_SUCCESS = 0,
	SP_FAILURE = 1
};

namespace SpConsole {
	void PlainWrite(const char* message);
	void Write(MessageSeverity severity, std::string message);

	void FatalExit(const char* message, ExitCode code);


	void VulkanResult(VkResult result,
	                  MessageSeverity successSeverity,
	                  MessageSeverity failSeverity,
	                  const char* successMessage,
	                  const char* failMessage);
	/*!
	 * Always uses the successSeverity "Info"
	 */
	void VulkanResult(VkResult result,
	                  MessageSeverity failSeverity,
	                  const char* successMessage,
	                  const char* failMessage);
	/*!
	 * Has no Success message
	 */
	void VulkanResult(VkResult result, MessageSeverity failSeverity, const char* failMessage);

	void VulkanExitCheck(VkResult result,
	                     MessageSeverity successSeverity,
	                     const char* successMessage,
	                     const char* failMessage,
	                     ExitCode code);
	/*!
	* Always uses the successSeverity "Info"
	*/
	void VulkanExitCheck(VkResult result, const char* successMessage, const char* failMessage, ExitCode code);
	/*!
	 * Has no Success message
	 */
	void VulkanExitCheck(VkResult result, const char* failMessage, ExitCode code);


	VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void sdlErrorCheck(bool result);
}

namespace RendUtils {
}

namespace Utils {
	class FileUtils {
		public:

		static std::vector<char> readBinaryFile(std::filesystem::path filePath);
		void writeBinaryFile(std::filesystem::path filePath, std::vector<char>& data);

		static std::vector<char> readTextFile(std::filesystem::path filePath);
		void writeTextFile(std::filesystem::path filePath, std::vector<char>& data);
	private:

	};
}

#endif //SPARKER_ENGINE_UTILS_H
