#ifndef PATHTRACER_UTIL_H
#define PATHTRACER_UTIL_H

#include <chrono>
#include <thread>

constexpr float MS_PER_NS = 1.f / 1000000.f;

// Basically a macro, forced inline for time efficiency
inline static unsigned long long NOW_NS() {
    return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now())
            .time_since_epoch()
            .count();
}

inline static unsigned long long NOW_MS() {
    return NOW_NS() * MS_PER_NS;
}

inline static void DELAY(unsigned long long nanos) {
    unsigned long long start = NOW_NS(), now = start;
    while (now - start < nanos) {
        std::this_thread::yield();
        now = NOW_NS();
    }
}

#endif //PATHTRACER_UTIL_H
