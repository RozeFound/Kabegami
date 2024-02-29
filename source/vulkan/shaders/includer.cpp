#include "includer.hpp"

namespace glsl {

    Includer::IncludeResult* Includer::includeSystem(const char* header, const char* includer, size_t depth) {
        // TODO: This should be used if a shader file says "#include <source>",
        // in which case it includes a "system" file instead of a local file.
        loge("GlslShaderIncluder::includeSystem() is not implemented!");
        logv("includeSystem({}, {}, {})", header, includer, depth);
        return nullptr;
    }

    Includer::IncludeResult* Includer::includeLocal(const char* header, const char* includer, size_t depth) {
        logv("includeLocal({}, {}, {})", header, includer, depth);

        if (includes.contains(header))
            return includes[header].get();

        if (!fs.exists(header)) {
            loge("Header not found: {}", header);
            return nullptr;
        }

        sources[header] = fs.read_as_string(header);
        includes[header] = std::make_unique<IncludeResult>(header, sources.at(header).c_str(), sources.at(header).size(), nullptr);

        return includes[header].get();

    }

    void Includer::releaseInclude(IncludeResult* result) {
        includes.erase(result->headerName);
        sources.erase(result->headerName);
    }

}