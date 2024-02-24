#include <thread>

#include "fps_limiter.hpp"

namespace vku {

    void FPSLimiter::set_target (int framerate) {

        if (framerate <= 0) interval = duration_t::zero();
        else interval = duration_t(duration_t::period::den / framerate);
        last_frame = hrc::now();

    }

    void FPSLimiter::delay ( ) {

        if (!is_enabled) return;

        auto current = hrc::now();
        auto frame_time = current - last_frame;

        auto sleep_duration = interval - deviation - frame_time;
        std::this_thread::sleep_for(sleep_duration);

        deviation += frame_time - interval;
        deviation = std::min(deviation, interval / 16);

        last_frame = current;

    }

}