#pragma once

#include <unordered_map>

#include <glslang/Public/ShaderLang.h>

#include "assets/filesystem.hpp"

namespace glsl {

    class Includer : public glslang::TShader::Includer {

    const assets::FileSystem& fs;

    std::unordered_map<std::string_view, std::unique_ptr<IncludeResult>> includes;
    std::unordered_map<std::string_view, std::string> sources;


    public:

    Includer (const assets::FileSystem& fs) : fs(fs) {}

    // Note "local" vs. "system" is not an "either/or": "local" is an
    // extra thing to do over "system". Both might get called, as per
    // the C++ specification.
    //
    // For the "system" or <>-style includes; search the "system" paths.
    virtual IncludeResult* includeSystem (const char*, const char*, size_t) override;

    // For the "local"-only aspect of a "" include. Should not search in the
    // "system" paths, because on returning a failure, the parser will
    // call includeSystem() to look in the "system" locations.
    virtual IncludeResult* includeLocal (const char*, const char*, size_t) override;

    virtual void releaseInclude(IncludeResult*) override;

};

}