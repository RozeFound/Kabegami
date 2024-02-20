#include "package.hpp"

namespace assets {

    Package::Package (std::filesystem::path path) {

        file = std::ifstream(path, std::ios::binary);

        auto package_version = read_string();
        logi("Package version: {}", package_version);

        auto entries_count = read_int();

        for (std::size_t i = 0; i < entries_count; i++) {

            auto entry_name = read_string();
            
            entries[entry_name].offset = read_int();
            entries[entry_name].length = read_int();

        }

        data_offset = file.tellg();

    }

    uint32_t Package::read_int() {

        uint32_t result; 
        file.read(reinterpret_cast<char*>(&result), sizeof(uint32_t));
        return result;

    }

    std::string Package::read_string() {

        auto size = read_int();

        std::string result;
        result.resize(size);

        file.read(result.data(), size);

        return result;

    }

    std::vector<std::byte> Package::read_file (std::string_view path) const {

        auto& entry = entries.at({ path.begin(), path.end() });
        auto result = std::vector<std::byte>(entry.length);

        file.seekg(data_offset + entry.offset);

        file.read(reinterpret_cast<char*>(result.data()), entry.length);
        
        return result;

    }

}