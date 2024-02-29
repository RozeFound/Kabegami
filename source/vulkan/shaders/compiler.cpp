#include "compiler.hpp"

namespace glsl {

    constexpr EShLanguage find_shader_language(vk::ShaderStageFlagBits stage) {
        switch (stage)
        {
            case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
            case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
            case vk::ShaderStageFlagBits::eCompute: return EShLangCompute;
            default: return EShLangVertex;
        }
    }

    bool Compiler::compile (std::vector<uint8_t> glsl_source) {

        glslang::InitializeProcess();

        EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

        auto language = find_shader_language(stage);
        auto resources = GetDefaultResources();
        auto profile = ECoreProfile;

        auto shader = glslang::TShader(language);

        auto source = std::string(glsl_source.begin(), glsl_source.end());
        const char* sources[1] = { source.c_str() };
        shader.setStrings(sources, 1);

        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
        shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_6);

        shader.setEntryPoint("main");

        shader.setPreamble("#extension GL_ARB_shading_language_include : require\n");

        logv("Compiling shader: {}", source);

        std::string processed_source;

        if (!shader.preprocess(resources, 460, profile, false, false, messages, &processed_source, includer)) {
            info_log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
            return false;
        }

        logv("Preprocessed shader: {}", processed_source);

        const char* preprocessed_sources[1] = { processed_source.c_str() };
        shader.setStrings(preprocessed_sources, 1);

        if (!shader.parse(resources, 460, profile, false, false, messages, includer)) {
            info_log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
            return false;
        }

        auto program = glslang::TProgram();
        program.addShader(&shader);

        if (!program.link(messages)) {
            info_log += std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());
            return false;
        }

        if (shader.getInfoLog())
            info_log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
        if (program.getInfoLog())
            info_log += std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());

        auto intermediate = program.getIntermediate(language);

        if (!intermediate) {
            loge("Failed to get shared intermeidate code");
            return false;
        }

        spv::SpvBuildLogger logger;
        glslang::SpvOptions options;
        options.validate = true;

        glslang::GlslangToSpv(*intermediate, spirv, &logger, &options);

        glslang::FinalizeProcess();

        info_log += logger.getAllMessages() + "\n";

        return true;
    }


}