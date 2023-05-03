#include "logging.hpp"

VKAPI_ATTR constexpr VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, 
    VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {

    auto type_flags = static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type);
    auto severity_flags = static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity); 

    using enum vk::DebugUtilsMessageTypeFlagBitsEXT;
    using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;

    if (severity_flags == eError) loge("Vulkan Error: {}", pCallbackData->pMessage);
    if (severity_flags == eWarning) logw("Vulkan Warning: {}", pCallbackData->pMessage);
    if (severity_flags == eInfo) logi("Vulkan Info: {}", pCallbackData->pMessage);
    if (severity_flags == eVerbose) logv("Vulkan: {}", pCallbackData->pMessage);

    return VK_FALSE;
}

std::unique_ptr<vk::raii::DebugUtilsMessengerEXT>
    make_debug_messenger (const vk::raii::Instance& instance) {

    using enum vk::DebugUtilsMessageTypeFlagBitsEXT;
    using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;

    auto create_info = vk::DebugUtilsMessengerCreateInfoEXT {
            .flags = vk::DebugUtilsMessengerCreateFlagsEXT(),
            .messageSeverity = eVerbose | eInfo | eWarning | eError,
            .messageType = eGeneral | eValidation | ePerformance,
            .pfnUserCallback = debug_callback, .pUserData = nullptr
    };

    return std::make_unique<vk::raii::DebugUtilsMessengerEXT>(instance, create_info);

}