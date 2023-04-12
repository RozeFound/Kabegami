#include <vector>
#include <string_view>
#include <memory>

#include "app.hpp"

auto main (const int argc, const char* const* const argv) -> int {

    const auto args = std::vector<std::string_view>(argv, argv + argc);
    
    auto app = std::make_unique<Kabegami>();

    app->run();

    return 0;

}