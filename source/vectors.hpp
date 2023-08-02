#pragma once

#include <string_view>

#include <glaze/glaze.hpp>

struct vec_t {

    constexpr virtual double& operator[] (const std::ptrdiff_t index) = 0;
    constexpr const double& operator[] (const std::ptrdiff_t index) const { return operator[](index); }

    constexpr double& at (const std::ptrdiff_t index) { return operator[](index); }
    constexpr const double& at (const std::ptrdiff_t index) const { return operator[](index); }

    constexpr virtual std::string string() const = 0;
    constexpr virtual std::size_t size() const noexcept = 0;

    constexpr double* begin() noexcept { return (double*)this + 1; };
    constexpr double* end() noexcept { return (double*)this+size() + 1; };

};

struct vec2_t : public vec_t {

    double x, y;

    constexpr double& operator[] (const std::ptrdiff_t index) override {

        switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Out of range vec2_t");
        }
    }

    constexpr std::string string() const override { return fmt::format("{:.2f}, {:.2f}", x, y); }
    constexpr std::size_t size() const noexcept override { return 2; }

};

struct vec3_t : public vec_t {

    union { double x, r; };
    union { double y, g; };
    union { double z, b; };

    constexpr operator glm::vec3() const noexcept { return { x, y, z }; }

    constexpr double& operator[] (const std::ptrdiff_t index) override {

        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Out of range vec3_t");
        }
    }

    constexpr std::string string() const override { return fmt::format("{:.2f}, {:.2f}, {:.2f}", x, y, z); }
    constexpr std::size_t size() const noexcept override { return 3; }

};

namespace glz::detail {

    template <class T> requires std::derived_from<T, vec_t> 
    struct from_json <T> {

        template <auto Opts>
        static void op (T& vec, auto&&... args) {

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