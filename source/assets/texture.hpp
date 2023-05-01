#pragma once

#include <vector>
#include <string_view>
#include <unordered_map>
#include <cstring>
#include <span>

class Reader {

    std::span<std::byte> data;
    std::ptrdiff_t offset = 0;

    public:

    Reader (std::span<std::byte> data) : data(data) {}

    constexpr const auto get_span() const { return data; }
    constexpr std::ptrdiff_t& get_offset() { return offset; }

    template<typename T = const char*>
    constexpr const auto get_range (std::ptrdiff_t range) const { 
        return reinterpret_cast<T>(data.subspan(offset, range).data());
    }

    template <typename T> T get() {
        T result;
        std::memcpy(&result, &data[offset], sizeof(T));
        offset += sizeof(T);
        return result;
    };

    template <> std::string_view get() {
        constexpr std::size_t string_length = 9;
        auto result = (char*)data.subspan(offset, string_length).data();
        offset += string_length;
        return result;
    };

};

struct Header {

    Header (Reader& reader);

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
        RGB161616f = 15
    } format;

    enum class Flags : uint32_t {
        NoFlags = 0,
        NoInterpolation = 1,
        ClampUVs = 2,
        IsGif = 4
    } flags;

    uint32_t texture_width;
    uint32_t texture_height;

    uint32_t width;
    uint32_t height;

};

struct MipMap {

    MipMap (Reader& reader, std::string_view container_version);

    uint32_t width;
    uint32_t height;

    bool compression = false;
    uint32_t size;

    std::vector<std::byte> data;

};

struct Frame {

    Frame (Reader& reader);

    uint32_t index;
    uint32_t duration;
    
    uint32_t x, y;

    uint32_t width;
    uint32_t height;

};

class Texture {

    Reader reader;

    Header header;

    std::string_view container_version;
    std::string_view animation_version;

    std::unordered_map<uint32_t, std::vector<MipMap>> images;

    uint32_t gif_width, gif_height;
    std::vector<Frame> frames;

    public:

    Texture (std::span<std::byte> data);

    constexpr const Header& get_header() const { return header; }

};