#include "compiler.hpp"

namespace glsl {

    constexpr EShLanguage Compiler::find_shader_language(vk::ShaderStageFlagBits stage) {
        switch (stage)
        {
            case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
            case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
            case vk::ShaderStageFlagBits::eCompute: return EShLangCompute;
            default: return EShLangVertex;
        }
    }

    bool Compiler::compile (std::string_view source) {

        glslang::InitializeProcess();

        EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

        auto language = find_shader_language(stage);
        auto resources = GetDefaultResources();
        auto profile = ECoreProfile;

        auto shader = glslang::TShader(language);

        const char* sources[1] = { source.data() };
        shader.setStrings(sources, 1);

        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
        shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_6);

        shader.setEntryPoint("main");

        logd("Compiling shader:\n{}", source);

        std::string processed_source;

        auto includer = glslang::TShader::ForbidIncluder{};

        if (!shader.preprocess(resources, 460, profile, false, false, messages, &processed_source, includer)) {
            log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
            return false;
        }

        logd("Preprocessed shader:\n{}", processed_source);

        const char* preprocessed_sources[1] = { processed_source.c_str() };
        shader.setStrings(preprocessed_sources, 1);

        if (!shader.parse(resources, 460, profile, false, false, messages)) {
            log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
            return false;
        }

        auto program = glslang::TProgram();
        program.addShader(&shader);

        if (!program.link(messages)) {
            log += std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());
            return false;
        }

        if (shader.getInfoLog())
            log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
        if (program.getInfoLog())
            log += std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());

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

        log += logger.getAllMessages() + "\n";

        return true;
    }


}