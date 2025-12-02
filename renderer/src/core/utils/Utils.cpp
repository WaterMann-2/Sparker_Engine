//
// Created by robsc on 10/19/25.
//
#include "Utils.h"


namespace SpConsole {
    void PlainWrite(const char* message) {
        std::cout << message << std::endl;
    }

    void Write(MessageSeverity severity, std::string message) {
        std::string suffix;
        switch ( severity ) {
            case SP_MESSAGE_VERBOSE:
                suffix = "[Verbose] " + std::string(message);
                std::cout << suffix << std::endl;
                break;
            case SP_MESSAGE_INFO:
                suffix = "[Info] " + std::string(message);
                std::cout << suffix << std::endl;
                break;
            case SP_MESSAGE_ERROR:
                suffix = "[Error] " + std::string(message);
                std::cout << suffix << std::endl;
                break;
            case SP_MESSAGE_WARNING:
                suffix = "[Warning] " + std::string(message);
                std::cout << suffix << std::endl;
                break;
            case SP_MESSAGE_FATAL:
                suffix = "[Fatal] " + std::string(message);
                std::cout << suffix << std::endl;
                break;
        }
    }

    void FatalExit(const char* message, ExitCode code) {
        Write(SP_MESSAGE_FATAL, message);
        exit(code);
    }

    void VulkanResult(VkResult result,
                      MessageSeverity successSeverity,
                      MessageSeverity failSeverity,
                      const char* successMessage,
                      const char* failMessage) {
        if ( result == VK_SUCCESS ) { Write(successSeverity, successMessage); }
        else {
            Write(failSeverity, failMessage);
            Write(failSeverity, ("Result Code: " + std::to_string(result)).c_str());
        }
    }

    void VulkanResult(VkResult result,
                      MessageSeverity failSeverity,
                      const char* successMessage,
                      const char* failMessage) {
        if ( result == VK_SUCCESS ) { Write(SP_MESSAGE_INFO, successMessage); }
        else {
            Write(failSeverity, failMessage);
            Write(failSeverity, ("Result Code: " + std::to_string(result)).c_str());
        }
    }

    void VulkanResult(VkResult result, MessageSeverity failSeverity, const char* failMessage) {
        if ( result != VK_SUCCESS ) {
            Write(failSeverity, failMessage);
            Write(failSeverity, ("Result Code: " + std::to_string(result)).c_str());
        }
    }


    void VulkanExitCheck(VkResult result,
                         MessageSeverity successSeverity,
                         const char* successMessage,
                         const char* failMessage,
                         ExitCode code) {
        if ( result == VK_SUCCESS ) { Write(successSeverity, successMessage); }
        else {
            Write(SP_MESSAGE_FATAL, failMessage);
            Write(SP_MESSAGE_FATAL, ("Result Code: " + std::to_string(result)).c_str());
            exit(code);
        }
    }

    void VulkanExitCheck(VkResult result, const char* successMessage, const char* failMessage, ExitCode code) {
        if ( result == VK_SUCCESS ) { Write(SP_MESSAGE_INFO, successMessage); }
        else {
            Write(SP_MESSAGE_FATAL, failMessage);
            Write(SP_MESSAGE_FATAL, ("Result Code: " + std::to_string(result)).c_str());
            exit(code);
        }
    }

    void VulkanExitCheck(VkResult result, const char* failMessage, ExitCode code) {
        if ( result != VK_SUCCESS ) {
            Write(SP_MESSAGE_FATAL, failMessage);
            Write(SP_MESSAGE_FATAL, ("Result Code: " + std::to_string(result)).c_str());
            exit(code);
        }
    }

    VkBool32 vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                             void* pUserData) {
        std::string message;
        switch (messageSeverity) {


            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                message =  std::string("[Verbose] [Vulkan] ") + pCallbackData->pMessage;
                PlainWrite(message.c_str());
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                message =  std::string("[Info] [Vulkan] ") + pCallbackData->pMessage;
                PlainWrite(message.c_str());
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                message =  std::string("[Warning] [Vulkan] ") + pCallbackData->pMessage;
                PlainWrite(message.c_str());
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                message =  std::string("[Error] [Vulkan] ") + pCallbackData->pMessage;
                PlainWrite(message.c_str());
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                message =  std::string("[Fatal] [Vulkan] ") + pCallbackData->pMessage;
                FatalExit(message.c_str(), SP_FAILURE);
                break;

        }

        return VK_FALSE;
    }

    void sdlErrorCheck(bool result) {
        if (!result) {
            const char* sdlError = SDL_GetError();
            std::string message = std::string("[SDL ERROR] ") + sdlError;
            PlainWrite(message.c_str());
        }
    }
}

std::vector<char> Utils::FileUtils::readBinaryFile(std::filesystem::path filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        SpConsole::Write(SP_MESSAGE_ERROR, "Failed to open file \"" + filePath.string() + "\"");

        if (file.bad()) SpConsole::FatalExit("Badbit is set!", SP_FAILURE);
        if (file.fail()) {
            std::string message = strerror(errno);
            SpConsole::FatalExit(message.c_str(), SP_FAILURE);
        }

        return {};
    }else {
        SpConsole::Write(SP_MESSAGE_INFO, "Opened File");
    }

    size_t fileSize = file.tellg();

    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void Utils::FileUtils::writeBinaryFile(std::filesystem::path filePath, std::vector<char>& data) {
    std::ofstream file(filePath, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        SpConsole::Write(SP_MESSAGE_ERROR, "Failed to open file \"" + filePath.string() + "\"");

        if (file.bad()) SpConsole::FatalExit("Badbit is set!", SP_FAILURE);
        if (file.fail()) {
            std::string message = strerror(errno);
            SpConsole::FatalExit(message.c_str(), SP_FAILURE);
        }

        return;
    }else {
        SpConsole::Write(SP_MESSAGE_INFO, "Opened File");
    }

    file.write(static_cast<char*>(data.data()), sizeof(char) * data.size());
    file.close();

    if (!file.good()) {
        SpConsole::FatalExit("Failed to write to file!", SP_FAILURE);
    }
}

std::vector<char> Utils::FileUtils::readTextFile(std::filesystem::path filePath) {
    std::ifstream file(filePath, std::ios::ate);

    if (!file.is_open()) {
        SpConsole::Write(SP_MESSAGE_ERROR, "Failed to open file \"" + filePath.string() + "\"");

        if (file.bad()) SpConsole::FatalExit("Badbit is set!", SP_FAILURE);
        if (file.fail()) {
            std::string message = strerror(errno);
            SpConsole::FatalExit(message.c_str(), SP_FAILURE);
        }

        return {};
    }else {
        SpConsole::Write(SP_MESSAGE_INFO, "Opened File");
    }

    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    std::string testText(buffer.begin(), buffer.end());

    SpConsole::Write(SP_MESSAGE_INFO, "Read Text");
    SpConsole::Write(SP_MESSAGE_INFO, testText);
    SpConsole::FatalExit("Test End", SP_SUCCESS);
}
