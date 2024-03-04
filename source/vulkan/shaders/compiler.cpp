#include "compiler.hpp"

namespace glsl {

    bool Compiler::preprocess (assets::ShaderUnit& unit, glslang::TShader& shader, Options options) {

        constexpr auto profile = EProfile::ECoreProfile;
        auto includer = glslang::TShader::ForbidIncluder{};

        if (!shader.preprocess(resource_limits, 110, profile, false, false, messages, &unit.source, includer)) {
            auto tmp_path = vku::fs::write_temp(unit.source);
            logi("--- shader preprocess failed ---");
            loge("shader source is at {}", tmp_path.string());
            loge("glslang(parse): {}", shader.getInfoLog());
            logi("--- end ---");
            return false;
        }

        return true;

    }

    bool Compiler::parse (assets::ShaderUnit& unit, glslang::TShader& shader, Options options) {

        auto* data   = unit.source.c_str();
        auto  client = get_client(options.client_version);

        shader.setStrings(&data, 1);
        shader.setEnvInput(options.hlsl ? glslang::EShSourceHlsl : glslang::EShSourceGlsl,
                        EShLanguage::EShLangVertex, client, 100);
        shader.setEnvClient(client, options.client_version);
        shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, get_target_version(options.client_version));

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

    bool Compiler::compile (std::array<assets::ShaderUnit, 2>& units) {

        glslang::InitializeProcess();

        Options options;

        glslang::TProgram program;

        std::vector<std::unique_ptr<glslang::TShader>> shaders;

        for (auto& unit : units) {
            auto language = find_shader_language(unit.stage);
            shaders.emplace_back(std::make_unique<glslang::TShader>(language));
            auto& shader = *shaders.back();
            if (!preprocess(unit, shader, options)) return false;
            if (!parse(unit, shader, options)) return false;
            program.addShader(&shader);
        }

        if (!program.link(messages)) {
            loge("glslang(link): {}\n", program.getInfoLog());
            return false;
        }

        auto language = find_shader_language(units.front().stage);

        auto* intermediate = program.getIntermediate(language);
        auto resolver = glslang::TDefaultGlslIoResolver(*intermediate);
        auto io_mapper = glslang::TGlslIoMapper();

        if (!(program.mapIO(&resolver, &io_mapper))) {
            loge("glslang(mapIo): {}\n", program.getInfoLog());
            return false;
        }

        spv::SpvBuildLogger logger;
        glslang::SpvOptions spv_options;
        spv_options.validate = true;
        spv_options.generateDebugInfo = false;

        for (auto& unit : units) {
            auto language = find_shader_language(unit.stage);
            auto intermediate = program.getIntermediate(language);
            intermediate->setOriginUpperLeft();
            auto spirv = std::vector<uint32_t>();
            glslang::GlslangToSpv(*intermediate, spirv, &logger, &spv_options);
            spvs.emplace(unit.stage, std::move(spirv));

            auto messages = logger.getAllMessages();
            if (messages.length() > 0) loge("glslang(spv): {}\n", messages);
        }

        return true;

    }


}