#include "texture.hpp"

#include <cstring>

Texture::Header::Header (std::span<std::byte> data) {

    constexpr std::size_t string_length = 9;
    std::ptrdiff_t offset = string_length;

    version = reinterpret_cast<char*>(data.subspan(0, string_length).data());
    index = reinterpret_cast<char*>(data.subspan(offset, string_length).data());
    offset += string_length;

    auto read_to = [&] <typename T> (T& result) {
        std::memcpy(&result, &data[offset], sizeof(T));
        offset += sizeof(T);
    };

    read_to(format);
    read_to(flags);

    read_to(texture_width);
    read_to(texture_height);

    read_to(width);
    read_to(height);

}