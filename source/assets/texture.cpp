#include "texture.hpp"

#include <lz4.h>
#include <stb_image.h>

namespace assets {

    Header::Header (Reader& reader) {

        version = reader.get<std::string_view>();
        index = reader.get<std::string_view>();

        format = reader.get<Format>();
        flags = reader.get<Flags>();

        texture_width = reader.get<uint32_t>();
        texture_height = reader.get<uint32_t>();

        width = reader.get<uint32_t>();
        height = reader.get<uint32_t>();

        reader.skip<uint32_t>(); // Unknown;

        container = reader.get<std::string_view>();
        image_count = reader.get<uint32_t>();

        if (container == "TEXB0003")
            type = reader.get<Type>(); // FreeImageFormat

    }

    MipMap::MipMap (Reader& reader, const Header& header) {

        width = reader.get<uint32_t>();
        height = reader.get<uint32_t>();

        if (header.container == "TEXB0002" || header.container == "TEXB0003") {

            compression = reader.get<uint32_t>();
            auto compressed = reader.get<uint32_t>();
            pixels.reserve(compressed);

        }

        size = reader.get<uint32_t>();

        if (compression) {
            LZ4_decompress_safe(
                reader.get_range<const char*>(size), 
                reinterpret_cast<char*>(pixels.data()),
                size, pixels.capacity());
        } else {
            auto uncompressed = reader.get_range(size);
            pixels.assign(uncompressed.begin(), uncompressed.end());
        }

        if (header.container == "TEXB0003" && header.type != Header::Type::UNKNOWN) {

            int32_t width, height, channels;
            auto stbi_image = stbi_load_from_memory((stbi_uc*)pixels.data(),
                                        size, &width, &height, &channels, 4);
            auto data = reinterpret_cast<std::byte*>(stbi_image);
            pixels.assign(data, data + size);
            stbi_image_free(stbi_image);

        }

        reader.skip(size);

    }

    Frame::Frame (Reader& reader) {

        index = reader.get<uint32_t>();
        duration = reader.get<uint32_t>();

        x = reader.get<uint32_t>();
        y = reader.get<uint32_t>();

        width = reader.get<uint32_t>();
        height = reader.get<uint32_t>();

        reader.skip<uint32_t>(2);

    }

    TextureParser::TextureParser (std::span<std::byte> data) : reader(data), header(reader) {

        for (uint32_t image = 0; image < header.image_count; image++) {

            auto mipmap_count = reader.get<uint32_t>();
            auto mipmaps = std::vector<MipMap>();

            for (uint32_t i = 0; i < mipmap_count; i ++)
                mipmaps.emplace_back(reader, header);

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

}