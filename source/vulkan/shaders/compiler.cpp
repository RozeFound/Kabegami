#include "compiler.hpp"

#include "vulkan/utility/misc.hpp"

namespace glsl {

    constexpr auto find_shader_language (vk::ShaderStageFlagBits stage) {
        switch (stage)
        {
            case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
            case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
            case vk::ShaderStageFlagBits::eGeometry: return EShLangGeometry;
            default: return EShLangVertex;
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

        // Initialize glslang library.
        glslang::InitializeProcess();

        auto language = find_shader_language(unit.stage);
        auto shader = glslang::TShader(language);

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

        auto intermediate = program.getIntermediate(language);

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

        // Shutdown glslang library.
        glslang::FinalizeProcess();

        return true;

    }


}