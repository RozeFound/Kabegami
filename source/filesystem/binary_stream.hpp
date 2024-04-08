#pragma once

#include <filesystem>
#include "utils.hpp" // fuck off clang, if you don't know what a concept is, doesn't mean it doesn't exists

namespace fs {

    enum class access_flags {
        read,
        write
    };
    using enum access_flags;

    class BinaryStream {

        std::FILE* file;
        std::size_t size;
        access_flags mode;

        template <typename T> T read_int() {
            T value;
            std::fread(&value, sizeof(T), 1, file);
            return value;
        }
        
        template <typename T> T read_array (std::size_t size) {
            auto buffer = T();
            buffer.resize(size);
            std::fread(buffer.data(), sizeof(decltype(buffer[0])), size, file);
            return buffer;
        }

        constexpr std::string read_string (std::size_t size) {
            auto buffer = std::string(size, '\0');
            std::fgets(buffer.data(), size, file);
            return buffer;
        }

        template <typename T> void write_int (T value) {
            std::fwrite(&value, sizeof(T), 1, file);
        }

        template <typename T> void write_array (T value, std::size_t size) {     
            for (std::size_t i = 0; i < size; i++) write_int(value[i]);
        }

        void write_string (std::string value) {
            std::fputs(value.c_str(), file);
        }

    public:

        BinaryStream (std::filesystem::path path, access_flags mode) : mode(mode) {

            if (std::filesystem::exists(path))
                size = std::filesystem::file_size(path);

            if (mode == fs::read) 
                file = std::fopen(path.c_str(), "rb");
            else if (mode == fs::write)
                file = std::fopen(path.c_str(), "wb+");

            if (std::ferror(file)) {
                loge("Failed to open file {} for {}", path.string(), mode == fs::read ? "reading" : "writing");
            }

        }

        BinaryStream (BinaryStream&&) noexcept = default;
        BinaryStream (BinaryStream&) = delete;

        ~BinaryStream() { if (file) std::fclose(file); }

        template <typename T> T read (std::size_t size = 0, std::ptrdiff_t offset = 0) {
            if (mode != fs::read || !file) throw std::runtime_error("Cannot read from file");

            if (size == 0) size = this->size;
            if (offset) std::fseek(file, offset, SEEK_SET);

            if constexpr (std::is_integral_v<T>) return read_int<T>();
            else if constexpr (is_array_like<T>) return read_array<T>(size);
            else if constexpr (std::is_convertible_v<T, std::string>) return read_string(size);

        }
        
        template <typename T> void write (T value, std::size_t size = 0, std::ptrdiff_t offset = 0) {
            if (mode != fs::write || !file) throw std::runtime_error("Cannot write to file");

            if (size == 0) size = this->size;
            if (offset) std::fseek(file, offset, SEEK_SET);

            if constexpr (std::is_integral_v<T>) write_int(value);
            else if constexpr (is_array_like<T>) write_array(value.data(), value.size());
            else if constexpr (std::is_convertible_v<T, std::string>) write_string(value);
        }

        constexpr std::ptrdiff_t get_position() { return std::ftell(file); }

    };

}