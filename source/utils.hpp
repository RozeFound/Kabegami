#pragma once

#include <filesystem>
#include <fstream>

#include <xxh3.h>

template <class T> concept is_array_like = requires (T cls) { cls.size(); cls.data(); };
template <class T> concept array_like = is_array_like<T>;

namespace hash {

    XXH64_hash_t XXH3 (array_like auto data) {
        return XXH3_64bits(data.data(), data.size());
    }

}

namespace fs {

    inline std::filesystem::path get_cache_dir() {
        auto home = std::filesystem::path(std::getenv("HOME"));
        std::string_view cache = std::getenv("XDG_CACHE_HOME");
        auto cache_dir = cache.empty() ? home / ".cache" : cache;
        return cache_dir / "kabegami";
    }

    template <typename T = std::vector<std::byte>>
    T read (std::filesystem::path path, std::ptrdiff_t position, std::size_t size) {
        
        auto file = std::ifstream (path, std::ios::ate | std::ios::binary);

        if (size == 0) size = file.tellg();
        file.seekg(position);

        if constexpr (is_array_like<T>) {
            auto buffer = T();
            buffer.resize(size);
            file.read(reinterpret_cast<char*>(buffer.data()), size);
            return buffer;
        } else {
            T value;
            file.read(reinterpret_cast<char*>(&value), sizeof(T));
            return value;
        }

    }

    void write (std::filesystem::path path, array_like auto data) {
        auto file = std::ofstream(path, std::ios::trunc | std::ios::binary);
        file.write(reinterpret_cast<char*>(data.data()), data.size());
    }

    std::filesystem::path write_temp (array_like auto data) {

        auto hash = std::to_string(hash::XXH3(data));
        auto path = std::filesystem::temp_directory_path() / hash;

        write(path, data);

        return path;
    }

}