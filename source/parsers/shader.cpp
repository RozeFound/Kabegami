#include "shader.hpp"

#include <regex>

#include <glaze/glaze.hpp>

struct Material {
    std::string name;
    glz::json_t value;
};

struct Combo {
    std::string name;
    std::string value;
};

template <> struct glz::meta<Material> {

    using T = Material;

    static constexpr auto value = object(
        "material", &T::name,
        "default", &T::value
    );
};

template <> struct glz::meta<Combo> {

    using T = Combo;

    static constexpr auto value = object(
        "combo", &T::name,
        "default", number<&T::value>
    );
};

namespace parsers {

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

    std::string load_glsl_include (const fs::VFS& vfs, std::string_view source) {

        std::string_view::size_type start = std::string_view::npos;
        std::string_view::size_type pos = 0;
        std::string output;

        while(start = source.find("#include", pos), start != std::string_view::npos) {

            auto end = source.find("\n", start);

            auto sQ = source.find('\"', start);
            auto eQ = source.find('\"', sQ + 1);

            output.append(source.substr(pos, start - pos));

            auto name = source.substr(sQ + 1, eQ - sQ - 1);

            auto content = vfs.read<std::string>(fmt::format("shaders/{}", name));

            output.append(fmt::format("\n//-----include {}\n", name));
            output.append(load_glsl_include(vfs, content));
            output.append("\n//-----include end\n");

            pos = end;

        }

        output.append(source.substr(pos));
        return output;

    }

    void parse (glsl::ShaderUnit& unit,
        std::unordered_map<std::string, std::string>& combos,
        std::unordered_map<std::string, std::string>& aliases,
        std::unordered_map<std::string, glz::json_t>& values,
        std::vector<std::string>& textures) {

        auto source = std::string(unit.source);

        static auto re_unif = std::regex(R"(^\s*uniform\s(\w+)\s(\w+))", std::regex::ECMAScript);

        std::string line;
        auto source_stream = std::istringstream(source);

        constexpr auto opts = glz::opts{.error_on_unknown_keys = false};

        while (std::getline(source_stream, line)) {

            if (line.find("COMBO") != std::string::npos) {

                Combo combo;
                auto buffer = std::string_view(line).substr(line.find_first_of('{'));
                
                if (auto error = glz::read<opts>(combo, buffer)) {
                    logw("Failed to parse combo: {}", glz::format_error(error, buffer));
                    continue;
                }

                if (!combos.contains(combo.name))
                    combos[combo.name] = combo.value;

                continue;
            }

            std::smatch match;
            if (std::regex_search(line, match, re_unif)) {

                auto j_start = line.find_first_of('{');
                if (j_start == std::string::npos) continue;

                Material material;
                auto buffer = std::string_view(line).substr(j_start);

                if (auto error = glz::read<opts>(material, buffer)) {
                    logw("Failed to parse material json: {}", glz::format_error(error, buffer));
                    continue;
                }

                auto& type = match[1];
                auto name = std::string(match[2]);

                aliases[name] = material.name;

                if (material.value) {
                    if (type == "sampler2D") {
                        int t_id = std::stoi(name.substr(name.length()-1));
                        textures.at(t_id) = material.value.as<std::string>();
                    }
                    else if (!values.contains(material.name))
                        values[material.name] = material.value;
                }

            }

        }

        for (auto&&[name, value] : combos)
            unit.define(name, value);

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

        shader.setPreamble(unit.get_preamble().c_str());
        shader.addProcesses(unit.get_processes());

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

    std::string load_glsl_file (const fs::VFS& vfs, std::string_view path) {

        auto source = vfs.read<std::string>(path);
        auto new_source = std::string(source);

        std::string::size_type pos = 0;
        std::string            include;

         while (pos = source.find("#include", pos), pos != std::string::npos) {
            auto end = source.find_first_of('\n', pos);
            new_source.replace(pos, end - pos, end - pos, ' ');
            include.append(source.substr(pos, end - pos) + "\n");
            pos = end;
        }
        
        include = load_glsl_include(vfs, include);

        return include + new_source;

    }

    Shader::Shader(const fs::VFS& vfs, std::string path) {

        units.emplace_back(load_glsl_file(vfs, path + ".vert"), vk::ShaderStageFlagBits::eVertex);
        units.emplace_back(load_glsl_file(vfs, path + ".frag"), vk::ShaderStageFlagBits::eFragment);

        if (vfs.exists(path + ".geom"))
            units.emplace_back(load_glsl_file(vfs, path + ".geom"), vk::ShaderStageFlagBits::eGeometry);

        std::unordered_map<std::string, std::string> combos;  // Pass::combos
        std::unordered_map<std::string, std::string> aliases; // g_Uniform -> material
        std::unordered_map<std::string, glz::json_t> values;  // Pass::constantshadervalues
        std::vector<std::string> textures(3);                    // Pass::textures

        for (auto& unit : units) {
            parse(unit, combos, aliases, values, textures);
            unit.source = preprocess(unit);
        }

        map_io(units);

    }

}