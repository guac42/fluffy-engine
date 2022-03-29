#ifndef PATHTRACER_UTIL_H
#define PATHTRACER_UTIL_H

#include <ctime>
#include <thread>

#define NS_PER_SECOND (1000000000)
#define NS_PER_MS (1000000)

// Basically a macro, forced inline for time efficiency
inline static unsigned long long NOW() {
    struct timespec ts{};
    timespec_get(&ts, TIME_UTC);
    return (ts.tv_sec * NS_PER_SECOND) + ts.tv_nsec;
}

inline static void DELAY(unsigned long long nanos) {
    unsigned long long start = NOW(), now = start;
    while (now - start < nanos) {
        std::this_thread::yield();
        now = NOW();
    }
}

#endif //PATHTRACER_UTIL_H
