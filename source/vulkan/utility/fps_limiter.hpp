#pragma once

#include <chrono>

namespace vku {

    class FPSLimiter {

        using hrc = std::chrono::high_resolution_clock;
        using duration_t = std::chrono::nanoseconds;

        hrc::time_point last_frame;
        duration_t deviation = duration_t::zero();
        duration_t interval = duration_t::zero();

        public:

        bool is_enabled = false;

        void set_target (int framerate);
        void delay();

    };

}