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

        return output;

    }

    std::string map_io (glsl::ShaderUnit& unit) {

        auto source = preprocess(unit);
        std::string output;

        std::vector<Parameter> inputs;
        std::vector<Parameter> outputs;
        std::vector<Parameter> textures;
        std::vector<Parameter> uniforms;

        // Counters
        uint32_t in_c = 0, out_c = 0, texture_c = 0;

        static auto re_io = std::regex(R"(^\s*(in|out|uniform)\s(\w+)\s(\w+))", std::regex::ECMAScript);

        std::string line;
        auto source_stream = std::istringstream(source);

        while (std::getline(source_stream, line)) {

            if (line.empty()) continue;
            if (line.find("void main()") != std::string::npos) {
                auto pos = -line.length() -1 + source_stream.tellg();
                output.append(source.substr(pos));
                break;
            }   
                
            std::smatch match;
            if (std::regex_search(line, match, re_io)) {
                if (match[2] == "sampler2D") textures.push_back({match[2], match[3], texture_c++});
                else if (match[1] == "in") inputs.push_back({match[2], match[3], in_c++});
                else if (match[1] == "out") outputs.push_back({match[2], match[3], out_c++});
                else if (match[1] == "uniform") uniforms.push_back({match[2], match[3], 0});
                continue;
            }

            output.append(line + '\n');

        }

        std::ostringstream parameters;

        parameters << "\n\n";

        if (!inputs.empty()) {
            for (auto& input : inputs)
                parameters << fmt::format("layout(location={}) in {} {};\n", input.location, input.type, input.name);
            parameters << "\n\n";
        }
        
        if (!outputs.empty()) {
            for (auto& output : outputs)
                parameters << fmt::format("layout(location={}) out {} {};\n", output.location, output.type, output.name);
            parameters << "\n\n";
        }

        if (!textures.empty()) {
            for (auto& texture : textures)
                parameters << fmt::format("layout(set=0, binding={}) uniform {} {};\n", texture.binding, texture.type, texture.name);
            parameters << "\n\n";
        }

        if (!uniforms.empty()) {
            parameters << "layout(binding=0) uniform GLOBAL {\n";
            for (auto& uniform : uniforms)
                parameters << fmt::format("\t{} {};\n", uniform.type, uniform.name);
            parameters << "};\n\n";
        }

        parameters << "\n\n";

        output.insert(output.find("void main()"), parameters.str());

        logd("Mapped IO: \n{}", output);

        return output;  

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

        for (auto& unit : units)
            unit.source = map_io(unit);


    }

}