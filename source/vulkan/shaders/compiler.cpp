#include "compiler.hpp"

#include "vulkan/utility/misc.hpp"

namespace glsl {

    void Compiler::set_messages() {
        auto messages = static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules);
        if (options.relaxed_errors_glsl) messages = (EShMessages)(messages | EShMsgRelaxedErrors);
        if (options.suppress_warnings_glsl) messages = (EShMessages)(messages | EShMsgSuppressWarnings);
        if (get_client(options.client_version) == glslang::EShClientVulkan)
            messages = (EShMessages)(messages | EShMsgVulkanRules);
        this->messages = messages;
    }

    constexpr glslang::EShClient Compiler::get_client (glslang::EShTargetClientVersion version) {
        switch (version) {
            case glslang::EShTargetVulkan_1_0:
            case glslang::EShTargetVulkan_1_1:
            case glslang::EShTargetVulkan_1_2:
            case glslang::EShTargetVulkan_1_3: return glslang::EShClientVulkan;
            case glslang::EShTargetOpenGL_450: return glslang::EShClientOpenGL;
            default: return glslang::EShClientVulkan;
        }
    }

    constexpr glslang::EShTargetLanguageVersion Compiler::get_target_version (glslang::EShTargetClientVersion version) {
        switch (version) {
            case glslang::EShTargetVulkan_1_0: return glslang::EShTargetSpv_1_0;
            case glslang::EShTargetVulkan_1_1: return glslang::EShTargetSpv_1_3;
            case glslang::EShTargetVulkan_1_2: return glslang::EShTargetSpv_1_5;
            case glslang::EShTargetVulkan_1_3: return glslang::EShTargetSpv_1_6;
            case glslang::EShTargetOpenGL_450: return glslang::EShTargetSpv_1_0;
            default: return glslang::EShTargetSpv_1_0;
        }
    }

    bool Compiler::parse (ShaderUnit& unit, glslang::TShader& shader) {

        auto* data   = unit.source.c_str();
        auto  client = get_client(options.client_version);

        shader.setStrings(&data, 1);
        shader.setEnvInput(options.hlsl ? glslang::EShSourceHlsl : glslang::EShSourceGlsl,
                           EShLanguage::EShLangVertex, client, 110);
        shader.setEnvClient(client, options.client_version);
        shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, get_target_version(options.client_version));

        shader.setPreamble(unit.get_preamble().c_str());
        shader.addProcesses(unit.get_processes());

        if (options.auto_map_locations) shader.setAutoMapLocations(true);
        if (options.auto_map_bindings) shader.setAutoMapBindings(true);

        if (options.relaxed_rules_vulkan) {
            shader.setGlobalUniformBinding(options.global_uniform_binding);
            shader.setEnvInputVulkanRulesRelaxed();
        }

        if (!shader.parse(resource_limits, 110, false, messages)) {
            auto tmp_path = vku::fs::write_temp(unit.source);
            logi("--- shader compile failed ---");
            loge("shader source is at {}", tmp_path.string());
            loge("glslang(parse): {}", shader.getInfoLog());
            logi("--- end ---");
            return false;
        }

        return true;

    }

    bool Compiler::compile (ShaderUnit& unit, std::vector<uint32_t>& spirv) {

        auto shader = glslang::TShader(unit.language);

        if (!parse(unit, shader))
            return false;

        // Add shader to new program object.
        glslang::TProgram program;
        program.addShader(&shader);

        // Link program.
        if (!program.link(messages)) {
            loge("glslang(link): {}", program.getInfoLog());
            return false;
        }

        auto intermediate = program.getIntermediate(unit.language);

        // Translate to SPIRV.
        if (!intermediate) {
            loge("Failed to get shared intermediate code.\n");
            return false;
        }

        spv::SpvBuildLogger logger;
        glslang::SpvOptions spv_options;

        if constexpr (!debug) {
            spv_options.disableOptimizer = !options.optimize;
            spv_options.optimizeSize = options.optimize_size;
        } else spv_options.validate = true;
        
        glslang::GlslangToSpv(*intermediate, spirv, &logger, &spv_options);

        auto log = logger.getAllMessages();
        if (log.size()) loge("glslang(spirv): {}", log);

        return true;

    }


}