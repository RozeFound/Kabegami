#include "engine.hpp"

Engine::Engine (const Window& window) {

    context = std::make_shared<vki::Context>(window);
    context->set(context);

}