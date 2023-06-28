#include "instance.hpp"

#include "vulkan/utility/misc.hpp"

namespace vki {

    Instance::Instance (const vk::raii::Context& context) {

        auto application_info = vk::ApplicationInfo {
            .pApplicationName = "Kabegami",
            .apiVersion = VK_API_VERSION_1_3
        };

        auto extensions = get_extensions();
        auto layers = get_layers();
        log_properties(context);

        auto validation_enable = vk::ValidationFeatureEnableEXT { vk::ValidationFeatureEnableEXT::eBestPractices };
        auto validation_features = vk::ValidationFeaturesEXT { .enabledValidationFeatureCount = 1, .pEnabledValidationFeatures = &validation_enable };

        auto instance_create_info = vk::InstanceCreateInfo {
            .pApplicationInfo = &application_info,
            .enabledLayerCount = vku::to_u32(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = vku::to_u32(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

        handle = std::make_unique<vk::raii::Instance>(context, instance_create_info);

    }

    std::vector <const char*> Instance::get_extensions() {

        uint32_t glfw_extension_count = 0;
        auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        auto extensions = std::vector(glfw_extensions, glfw_extensions + glfw_extension_count);
        if (!glfw_extensions) logw("Failed to fetch required GLFW Extensions!");

        if constexpr (debug) extensions.push_back("VK_EXT_debug_utils");

        logv("Extensions to be requested: ");
            for (auto& extension : extensions)
                logv("\t{}", extension);

        return extensions;

    }

    std::vector <const char*> Instance::get_layers() {

        auto layers = std::vector<const char*>();

        if constexpr (debug) layers.push_back("VK_LAYER_KHRONOS_validation");

        return layers;

    }

    void Instance::log_properties (const vk::raii::Context& context) {

        auto extensions = context.enumerateInstanceExtensionProperties();

        logv("Instance can support extensions:");
        for (const auto& extension : extensions)
            logv("\t{}", extension.extensionName);

        auto layers = context.enumerateInstanceLayerProperties();

        logv("Instance can support layers:");
        for (const auto& layer : layers)
            logv("\t{}", layer.layerName);

    }

}