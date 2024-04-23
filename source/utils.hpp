#pragma once

#include "filesystem/binary_stream.hpp"
#include <xxh3.h>

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

    std::filesystem::path write_temp (array_like auto data) {

        auto hash = std::to_string(hash::XXH3(data));
        auto path = std::filesystem::temp_directory_path() / hash;

        auto file = fs::FileBinaryStream(path, fs::write);
        file.write(data);

        return path;
    }

}