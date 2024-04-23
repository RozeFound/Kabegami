#pragma once

#include <filesystem>

template <class T> concept is_array_like = requires (T cls) { cls.size(); cls.data(); };
template <class T> concept array_like = is_array_like<T>;

namespace fs {

    enum class access_flags {
        read, write
    };

    enum class stream_type { 
        file, memory
    };

    using enum access_flags;
    using enum stream_type;

    namespace __impl {

        class MemoryBinaryStream {
        
            std::vector<std::byte> buffer;
            std::ptrdiff_t position = 0;

            access_flags mode;
            std::size_t size;

            template <typename T> T read_int() {
                T value;
                std::memcpy(&value, this->buffer.data() + position, sizeof(T));
                position += sizeof(T);
                return value;
            }
            
            template <typename T> T read_array (std::size_t size) {
                auto buffer = T();
                buffer.resize(size);
                std::memcpy(buffer.data(), this->buffer.data() + position, size);
                position += size;
                return buffer;
            }

            std::string read_string (std::size_t size) {
                auto buffer = std::string(size, '\0');
                std::memcpy(buffer.data(), this->buffer.data() + position, size);
                position += size;
                return buffer;
            }

            template <typename T> void write_int(T value) {
                std::memcpy(this->buffer.data() + position, &value, sizeof(T));
                position += sizeof(T);
            }
            
            template <typename T> void write_array(T value, std::size_t size) {
                for (std::size_t i = 0; i < size; i++) write_int(value[i]);
            }

            void write_string (std::string_view value) {
                std::memcpy(this->buffer.data() + position, value.data(), value.size());
                position += value.size();
            }

            public:

            MemoryBinaryStream (std::vector<std::byte> buffer, access_flags mode)
                : buffer(buffer), mode(mode), size(buffer.size()) {}

            template <typename T> T read (std::size_t size = 0, std::ptrdiff_t offset = 0) {
                if (mode != fs::read) throw std::runtime_error("Cannot read from memory");

                if (size == 0) size = get_size();
                if (offset) set_position(offset);

                if constexpr (std::is_integral_v<T>) return read_int<T>();
                else if constexpr (std::is_convertible_v<T, std::string>) return read_string(size);
                else if constexpr (is_array_like<T>) return read_array<T>(size);

            }

            template <typename T> void write (T value, std::size_t size = 0, std::ptrdiff_t offset = 0) {
                if (mode != fs::write) throw std::runtime_error("Cannot write to memory");

                if (size == 0) size = get_size();
                if (offset) set_position(offset);

                if constexpr (std::is_integral_v<T>) write_int(value);
                else if constexpr (std::is_convertible_v<T, std::string>) write_string(value);
                else if constexpr (is_array_like<T>) write_array(value.data(), value.size());

            }

            constexpr std::size_t get_size() const { return size; }

            constexpr std::ptrdiff_t get_position() const { return position; }
            constexpr void set_position (std::ptrdiff_t position) { this->position = position; }

        };

        class FileBinaryStream {

            std::FILE* file;

            access_flags mode;
            std::size_t size;

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
                std::fread(buffer.data(), sizeof(char), size, file);
                return buffer;
            }

            template <typename T> void write_int (T value) {
                std::fwrite(&value, sizeof(T), 1, file);
            }

            template <typename T> void write_array (T value, std::size_t size) {     
                for (std::size_t i = 0; i < size; i++) write_int(value[i]);
            }

            constexpr void write_string (std::string_view value) {
                std::fputs(value.data(), file);
            }

            public:

            FileBinaryStream (std::filesystem::path path, access_flags mode)
                : mode(mode), size(0) {

                if (mode == fs::read && std::filesystem::exists(path))
                    size = std::filesystem::file_size(path);

                if (mode == fs::read) 
                    file = std::fopen(path.c_str(), "rb");
                else if (mode == fs::write)
                    file = std::fopen(path.c_str(), "wb+");

                if (std::ferror(file)) {
                    loge("Failed to open file {} for {}", path.string(), mode == fs::read ? "reading" : "writing");
                }

            }

            ~FileBinaryStream() { if (file) std::fclose(file); }

            template <typename T> T read (std::size_t size = 0, std::ptrdiff_t offset = 0) {
                if (!file || mode != fs::read) throw std::runtime_error("Cannot read from file");

                if (size == 0) size = get_size();
                if (offset) set_position(offset);

                if constexpr (std::is_integral_v<T>) return read_int<T>();
                else if constexpr (std::is_convertible_v<T, std::string>) return read_string(size);
                else if constexpr (is_array_like<T>) return read_array<T>(size);

            }

            template <typename T> void write (T value, std::size_t size = 0, std::ptrdiff_t offset = 0) {
                if (!file || mode != fs::write) throw std::runtime_error("Cannot write to file");

                if (size == 0) size = get_size();
                if (offset) set_position(offset);

                if constexpr (std::is_integral_v<T>) write_int(value);
                else if constexpr (std::is_convertible_v<T, std::string>) write_string(value);
                else if constexpr (is_array_like<T>) write_array(value.data(), value.size());

            }

            constexpr std::size_t get_size() const { return size; }
            constexpr std::ptrdiff_t get_position() const { return std::ftell(file); }

            constexpr void set_position (std::ptrdiff_t offset) { std::fseek(file, offset, SEEK_SET); }

        };

    }

    class BinaryStream {

        protected:

        std::unique_ptr<__impl::FileBinaryStream> _file_impl;
        std::unique_ptr<__impl::MemoryBinaryStream> _memory_impl;

        stream_type type;

        public:

        BinaryStream() = delete;
        BinaryStream (stream_type type) : type(type) {}
        BinaryStream (BinaryStream&&) noexcept = default;
        BinaryStream (BinaryStream&) = delete;

        template <typename T> T read (std::size_t size = 0, std::ptrdiff_t offset = 0) {

            if (type == fs::file) return _file_impl->read<T>(size, offset);
            else if (type == fs::memory) return _memory_impl->read<T>(size, offset);
            else throw std::runtime_error("Unknown stream type");
        }

        template <typename T> void write (T value, std::size_t size = 0, std::ptrdiff_t offset = 0) {

            if (type == fs::file) _file_impl->write(value, size, offset);
            else if (type == fs::memory) _memory_impl->write(value, size, offset);
            else throw std::runtime_error("Unknown stream type");
        }

        std::ptrdiff_t get_position() const {

            if (type == fs::file) return _file_impl->get_position();
            else if (type == fs::memory) return _memory_impl->get_position();
            else throw std::runtime_error("Unknown stream type");
        }
        void set_position (std::ptrdiff_t position) {

            if (type == fs::file) _file_impl->set_position(position);
            else if (type == fs::memory) _memory_impl->set_position(position);
            else throw std::runtime_error("Unknown stream type");
        }

    };

    class MemoryBinaryStream : public BinaryStream {

        public:

        MemoryBinaryStream (std::vector<std::byte> buffer, access_flags mode)
            : BinaryStream(fs::memory) {
            _memory_impl = std::make_unique<__impl::MemoryBinaryStream>(buffer, mode);
        }
        
    };

    class FileBinaryStream : public BinaryStream {

        public:

        FileBinaryStream (std::filesystem::path path, access_flags mode)
            : BinaryStream(fs::file) {
            _file_impl = std::make_unique<__impl::FileBinaryStream>(path, mode);
        }

    };

}