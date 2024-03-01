#pragma once

#include <glaze/glaze.hpp>

namespace objects {

    struct Pass {

        int id;

        std::unordered_map<std::string_view, int> combos;

        std::string_view blending;
        std::string_view cullmode;
        std::string_view depthtest;
        std::string_view depthwrite;
        std::string_view shader;

        std::unordered_map<std::string_view, glz::json_t> constantshadervalues;
        std::vector<std::optional<std::string_view>> textures;

    };

    struct Material {
        std::vector<Pass> passes;
    };

}

template <> struct glz::meta<objects::Pass> {

    using T = objects::Pass;

    static constexpr auto value = object(

        "id", &T::id,
        "combos", &T::combos,
        "blending", &T::blending,
        "cullmode", &T::cullmode,
        "depthtest", &T::depthtest,
        "depthwrite", &T::depthwrite,
        "shader", &T::shader,
        "constantshadervalues", &T::constantshadervalues,
        "textures", &T::textures
    );

};
