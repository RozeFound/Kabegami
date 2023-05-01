#include "texture.hpp"

#include <lz4.h>

Header::Header (Reader& reader) {

    version = reader.get<std::string_view>();
    index = reader.get<std::string_view>();

    format = reader.get<Format>();
    flags = reader.get<Flags>();

    texture_width = reader.get<uint32_t>();
    texture_height = reader.get<uint32_t>();

    width = reader.get<uint32_t>();
    height = reader.get<uint32_t>();

    reader.get_offset() += sizeof(uint32_t);

}

MipMap::MipMap (Reader& reader, std::string_view container_version) {

    width = reader.get<uint32_t>();
    height = reader.get<uint32_t>();

    if (container_version == "TEXB0002" || container_version == "TEXB0003") {

        compression = reader.get<uint32_t>();
        auto compressed = reader.get<uint32_t>();
        data.reserve(compressed);

    }

    size = reader.get<uint32_t>();

    if (compression) {
        LZ4_decompress_safe(reader.get_range(size), 
            reinterpret_cast<char*>(data.data()),
            size, data.capacity());
    } else {
        auto uncompressed = reader.get_span().subspan(reader.get_offset(), size);
        data.assign(uncompressed.begin(), uncompressed.end());
    }

    reader.get_offset() += size;

}

Frame::Frame (Reader& reader) {

    index = reader.get<uint32_t>();
    duration = reader.get<uint32_t>();

    x = reader.get<uint32_t>();
    y = reader.get<uint32_t>();

    width = reader.get<uint32_t>();
    height = reader.get<uint32_t>();

    reader.get_offset()  += sizeof(uint32_t) * 2;

}

Texture::Texture (std::span<std::byte> data) : reader(data), header(reader) {

    container_version = reader.get<std::string_view>();
    auto image_count = reader.get<uint32_t>();

    if (container_version == "TEXB0003")
        reader.get_offset() += sizeof(uint32_t); // FreeImageFormat

    for (uint32_t image = 0; image < image_count; image++) {

        auto mipmap_count = reader.get<uint32_t>();
        auto mipmaps = std::vector<MipMap>();

        for (uint32_t i = 0; i < mipmap_count; i ++)
            mipmaps.emplace_back(reader, container_version);

        images[image] = mipmaps; // will make a copy?

    }

    if (header.flags == Header::Flags::IsGif) {

        animation_version = reader.get<std::string_view>();
        auto frame_count = reader.get<uint32_t>();

        if (animation_version == "TEXS0003") {
            gif_width = reader.get<uint32_t>();
            gif_height = reader.get<uint32_t>();
        }

        while (frame_count--)
            frames.emplace_back(reader);

        if (animation_version == "TEXS0002") {
            gif_width = frames.front().width;
            gif_height = frames.front().height;
        }

    }

}