#include "shader.hpp"

namespace assets {

    constexpr auto pre_shader_code = R"(
        #version 450

        #define GLSL 1
        #define HLSL 0

        precision highp float;

        #define CAST2(x) (vec2(x))
        #define CAST3(x) (vec3(x))
        #define CAST4(x) (vec4(x))
        #define CAST3X3(x) (mat3(x))

        #define texSample2D texture
        #define texSample2DLod textureLod
        #define mul(x, y) ((y) * (x))
        #define frac fract
        #define atan2 atan
        #define fmod(x, y) (x-y*trunc(x/y))
        #define ddx dFdx
        #define ddy(x) dFdy(-(x))
        #define saturate(x) (clamp(x, 0.0, 1.0))

        #define max(x, y) max(y, x)

        #define float1 float
        #define float2 vec2
        #define float3 vec3
        #define float4 vec4
        #define lerp mix
    )";

    std::string load_glsl_include (const FileSystem& fs, std::string_view source) {

        std::string_view::size_type start = std::string_view::npos;
        std::string_view::size_type pos = 0;
        std::string output;

        while(start = source.find("#include", pos), start != std::string_view::npos) {

            auto end = source.find("\n", start);

            auto sQ = source.find("\"", start);
            auto eQ = source.find("\"", sQ + 1);

            output.append(source.substr(pos, start - pos));

            auto name = source.substr(sQ + 1, eQ - sQ - 1);

            auto content = fs.read_as_string(fmt::format("shaders/{}", name));

            output.append(fmt::format("\n//-----include {}\n", name));
            output.append(load_glsl_include(fs, content));
            output.append("\n//-----include end\n");

            pos = end;

        }

        output.append(source.substr(pos));
        return output;

    }

    std::string ShaderParser::preprocess (glsl::ShaderUnit& unit) {

        auto header = std::string(pre_shader_code);

        if (unit.stage == vk::ShaderStageFlagBits::eVertex)
            header += "#define attribute in;\n"
                      "#defnie varying out;\n";
        else if (unit.stage == vk::ShaderStageFlagBits::eFragment)
            header += "#defnie varying in;\n"
                      "#define gl_FragColor diffuseColor\n"
                      "out vec4 diffuseColor;\n";

        constexpr auto profile = EProfile::ECoreProfile;
        auto includer = glslang::TShader::ForbidIncluder{};
        auto shader = glslang::TShader(unit.language);

        auto messages = (EShMessages)(EShMsgDefault | EShMsgSpvRules | EShMsgRelaxedErrors | EShMsgSuppressWarnings | EShMsgVulkanRules);

        auto* data = unit.source.c_str();
        std::string result;
        shader.setStrings(&data, 1);
        shader.preprocess(GetDefaultResources(), 110, profile, false, false, messages, &result, includer);

        return result;

    }

    std::string load_glsl_file (const FileSystem& fs, std::string_view path) {

        auto source = fs.read_as_string(path);
        auto new_source = std::string(source);

        std::string::size_type pos = 0;
        std::string            include;

         while (pos = source.find("#include", pos), pos != std::string::npos) {
            auto end = source.find_first_of('\n', pos);
            new_source.replace(pos, end - pos, end - pos, ' ');
            include.append(source.substr(pos, end - pos) + "\n");
            pos = end;
        }
        
        include = load_glsl_include(fs, include);

        return include + source;

    }

    ShaderParser::ShaderParser(std::string path, const FileSystem& fs) {

        units = {
            {
                .source = load_glsl_file(fs, path + ".vert"),
                .stage = vk::ShaderStageFlagBits::eVertex
            }, 
            {
                .source = load_glsl_file(fs, path + ".frag"),
                .stage = vk::ShaderStageFlagBits::eFragment
            }
        };

        units[0].source = preprocess(units[0]);
        units[1].source = preprocess(units[1]);

    }

}