//
// Created by robsc on 10/19/25.
//
#include "Utils.h"

namespace SpConsole {

    void Write(MessageSeverity severity, const char* message) {
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


        return VK_FALSE;
    }

}
