#include "shader.hpp"

#include <regex>

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

__SHADER_PLACEHOLD__

)";

    std::string load_glsl_include (const FileSystem& fs, std::string_view source) {

        std::string_view::size_type start = std::string_view::npos;
        std::string_view::size_type pos = 0;
        std::string output;

        while(start = source.find("#include", pos), start != std::string_view::npos) {

            auto end = source.find("\n", start);

            auto sQ = source.find('\"', start);
            auto eQ = source.find('\"', sQ + 1);

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

    std::string preprocess (glsl::ShaderUnit& unit) {

        auto header = std::string(pre_shader_code);

        if (unit.stage == vk::ShaderStageFlagBits::eVertex)
            header += "#define attribute in\n"
                      "#define varying out\n";     
        else if (unit.stage == vk::ShaderStageFlagBits::eFragment)
            header += "#define varying in\n"
                      "#define gl_FragColor diffuseColor\n"
                      "out vec4 diffuseColor;\n";

        auto source = std::string(header + unit.source);
        auto output = std::string();

        auto shader = glslang::TShader(unit.language);
        auto* data = source.c_str();
        shader.setStrings(&data, 1);

        constexpr auto profile = EProfile::ECoreProfile;
        auto includer = glslang::TShader::ForbidIncluder();
        auto messages = static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgRelaxedErrors | EShMsgSuppressWarnings | EShMsgVulkanRules);

        shader.preprocess(GetDefaultResources(), 110, profile, false, false, messages, &output, includer);
        
        static auto re_io = std::regex(R"(.+\s(in|out)\s[\s\w]+\s(\w+)\s*;)", std::regex::ECMAScript);
        std::smatch match;

        // iterate over matches with std::regex_iterator
        for (auto it = std::sregex_iterator(output.begin(), output.end(), re_io);
                  it != std::sregex_iterator(); ++it) {
            auto& match = *it; auto& io = match[1]; auto& name = match[2];
            if (io == "in") unit.preprocess_info.inputs[name] = match[0];
            else if (io == "out") unit.preprocess_info.outputs[name] = match[0];
        }

        return output;

    }

    void map_io (std::vector<glsl::ShaderUnit>& units) {
        
        for (std::size_t i = 0; i < units.size(); i++) {

            std::string io_list;
            auto& unit = units[i];

            if (i >= 1) {
                auto& info = units[i - 1].preprocess_info;
                for (auto& [name, io] : info.outputs) {
                    if (!unit.preprocess_info.inputs.contains(name)) {
                        auto nio = std::regex_replace(io, std::regex(R"(\s*out\s)"), " in ");
                        io_list += nio + '\n';
                    }            
                }
            }

            if (i + 1 < units.size()) {
                auto& info = units[i + 1].preprocess_info;
                for (auto& [name, io] : info.inputs) {
                    if (!unit.preprocess_info.outputs.contains(name)) {
                        auto nio = std::regex_replace(io, std::regex(R"(\s*in\s)"), " out ");
                        io_list += nio + '\n';
                    }
                }
            }

            unit.source = std::regex_replace(unit.source, std::regex("__SHADER_PLACEHOLD__"), io_list);

        }

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

        return include + new_source;

    }

    ShaderParser::ShaderParser(const FileSystem& fs, std::string path) {

        units.emplace_back(load_glsl_file(fs, path + ".vert"), vk::ShaderStageFlagBits::eVertex);
        units.emplace_back(load_glsl_file(fs, path + ".frag"), vk::ShaderStageFlagBits::eFragment);

        if (fs.exists(path + ".geom"))
            units.emplace_back(load_glsl_file(fs, path + ".geom"), vk::ShaderStageFlagBits::eGeometry);

        std::for_each(units.begin(), units.end(), [&](glsl::ShaderUnit& unit) {
            unit.source = preprocess(unit);
        });

        map_io(units);

    }

}