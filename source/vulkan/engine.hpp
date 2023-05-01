#pragma once

#include <memory>

#include "core/context.hpp"

#include "window.hpp"

class Engine {

    std::shared_ptr<vki::Context> context;

    public:

    Engine (const Window& window);

};