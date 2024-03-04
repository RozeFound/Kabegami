#include "shader.hpp"

namespace assets {

    constexpr auto pre_shader_code = R"(
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

    std::string resolve_includes(std::string_view source, const FileSystem& fs) {

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
            output.append(resolve_includes(content, fs));
            output.append("\n//-----include end\n");

            pos = end;

        }

        output.append(source.substr(pos));
        return output;

    }

    ShaderParser::ShaderParser(std::string path, const FileSystem& fs) {

        units[0] = {
            .source = pre_shader_code + resolve_includes(fs.read_as_string(path + ".vert"), fs),
            .stage = vk::ShaderStageFlagBits::eVertex
        };
            
        units[1] = {
            .source = pre_shader_code + resolve_includes(fs.read_as_string(path + ".frag"), fs),
            .stage = vk::ShaderStageFlagBits::eFragment
        };

        for (auto& unit : units) {
  
            if (unit.stage == vk::ShaderStageFlagBits::eVertex) {
                constexpr auto code = "#define attribute in;"
                                      "#defnie varying out;";
                unit.source = code + unit.source;
            } else {
                constexpr auto code = "#defnie varying in;"
                                      "out vec4 diffuseColor;";
                unit.source = code + unit.source;
            }
        }

    }

}