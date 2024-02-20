#pragma once

#include <string_view>

#include <glaze/glaze.hpp>

struct vec_t {

    constexpr virtual std::string string() const = 0;
    constexpr virtual std::size_t size() const noexcept = 0;

    constexpr double* begin() noexcept { return (double*)this + 1; };
    constexpr double* end() noexcept { return (double*)this+size() + 1; };

};

struct vec2_t : public vec_t {

    double x, y;

    constexpr std::string string() const override { return fmt::format("{:.2f}, {:.2f}", x, y); }
    constexpr std::size_t size() const noexcept override { return 2; }

};

struct vec3_t : public vec_t {

    union { double x, r; };
    union { double y, g; };
    union { double z, b; };

    constexpr operator glm::vec3() const noexcept { return { x, y, z }; }

    constexpr std::string string() const override { return fmt::format("{:.2f}, {:.2f}, {:.2f}", x, y, z); }
    constexpr std::size_t size() const noexcept override { return 3; }

};

template <class T> requires std::derived_from<T, vec_t>
struct glz::meta<T>
{
   static constexpr auto custom_read = true;
};

namespace glz::detail {

    template <class T> requires std::derived_from<T, vec_t> 
    struct from_json <T> {

        template <auto Opts>
        static void op (T& vec, auto&&... args) noexcept {

            char _cbuf[64]; 
            std::string_view buffer(_cbuf); 
            read<json>::op<Opts>(buffer, args...);

            for (int position = 0; auto& value : vec) {
                auto str = buffer.substr(position);
                position = buffer.find(' ', position) + 1;
                std::from_chars(str.begin(), str.end(), value);
            }

        }

    };
    
}