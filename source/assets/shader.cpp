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

    constexpr vk::ShaderStageFlagBits parse_stage(std::string_view path) {

        auto ext = path.substr(path.find_last_of("."));

        if (ext == ".vert") {
            return vk::ShaderStageFlagBits::eVertex;
        } else if (ext == ".frag") {
            return vk::ShaderStageFlagBits::eFragment;
        } else if (ext == ".comp") {
            return vk::ShaderStageFlagBits::eCompute;
        } else if (ext == ".tesc") {
            return vk::ShaderStageFlagBits::eTessellationControl;
        } else if (ext == ".tese") {
            return vk::ShaderStageFlagBits::eTessellationEvaluation;
        } else if (ext == ".geom") {
            return vk::ShaderStageFlagBits::eGeometry;
        } else {
            throw std::runtime_error("Unknown shader extension");
        }

    }

    ShaderParser::ShaderParser(std::string_view path, const FileSystem& fs)
        : path(path), stage(parse_stage(path)) {

        source = pre_shader_code + resolve_includes(fs.read_as_string(path), fs);

    }

}