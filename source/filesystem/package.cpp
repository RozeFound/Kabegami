#include "package.hpp"

#include <fmt/core.h>



Package::Package (std::filesystem::path path) {

    file = std::ifstream(path, std::ios::binary);

    fmt::print("Package version: {}\n", read_string());

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

std::vector<std::byte> Package::read_file (std::string_view path) {

    auto& entry = entries.at(path.data());
    auto result = std::vector<std::byte>(entry.length);

    file.seekg(data_offset + entry.offset);

    file.read(reinterpret_cast<char*>(result.data()), entry.length);
    
    return result;

}