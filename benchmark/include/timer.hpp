#pragma once

#include <chrono>
#include <cstdint>

namespace benchmark {

class Timer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<double, std::milli>;

    Timer() = default;

    void start() {
        start_ = Clock::now();
    }

    void stop() {
        end_ = Clock::now();
    }

    double elapsedMs() const {
        return Duration(end_ - start_).count();
    }

    double elapsedNs() const {
        auto ns = std::chrono::duration<double, std::nano>(end_ - start_);
        return ns.count();
    }

    static TimePoint now() {
        return Clock::now();
    }

    static double durationMs(TimePoint start, TimePoint end) {
        return Duration(end - start).count();
    }

private:
    TimePoint start_;
    TimePoint end_;
};

class ScopedTimer {
public:
    explicit ScopedTimer(double& result)
        : result_(result) {
        timer_.start();
    }

    ~ScopedTimer() {
        timer_.stop();
        result_ = timer_.elapsedMs();
    }

private:
    Timer timer_;
    double& result_;
};

} // namespace benchmark
