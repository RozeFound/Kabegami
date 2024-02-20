#pragma once

#include <glaze/glaze.hpp>

namespace objects {

    struct Sound {

        int id;
        bool visible;
        std::string_view name;

        std::vector<std::string_view> sound;
        std::string_view playbackmode;
        double volume;

        double mintime;
        double maxtime;

        bool muteineditor;
        bool startsilent;

    };

}

template <> struct glz::meta<objects::Sound> {

    using T = objects::Sound;

    static constexpr auto value = object (

        "id", &T::id,
        "visible", &T::visible,
        "name", &T::name,
        "sound", &T::sound,
        "playbackmode", &T::playbackmode,
        "volume", &T::volume,
        "mintime", &T::mintime,
        "maxtime", &T::maxtime,
        "muteineditor", &T::muteineditor,
        "startsilent", &T::startsilent,

        "locktransforms", skip{},
        "parallaxDepth", skip{},
        "angles", skip{},
        "origin", skip{},
        "scale", skip{}
    
    );

};