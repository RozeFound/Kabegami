#pragma once

#include <string_view>
#include <memory>
#include <span>

class Texture {

    struct Header {

        Header(std::span<std::byte> data);

        std::string_view version;
        std::string_view index;

        enum class Format : uint32_t {
            UNKNOWN = 0xFFFFFFFF,
            ARGB8888 = 0,
            RGB888 = 1,
            RGB565 = 2,
            DXT5 = 4,
            DXT3 = 6,
            DXT1 = 7,
            RG88 = 8,
            R8 = 9,
            RG1616f = 10,
            R16f = 11,
            BC7 = 12,
            RGBa1010102 = 13,
            RGBA16161616f = 14,
            RGB161616f = 15,
        } format;

        enum class Flags : uint32_t {
            NoFlags = 0,
            NoInterpolation = 1,
            ClampUVs = 2,
            IsGif = 4,
        } flags;

        uint32_t texture_width;
        uint32_t texture_height;

        uint32_t width;
        uint32_t height;

        uint32_t unknown;

    } header;

    public:

    Texture (std::span<std::byte> data) : 
        header(data.subspan(0, sizeof(Header))) {}

    constexpr const Header& get_header() const { return header; }

};