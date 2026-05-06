#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <mutex>

namespace benchmark {

class MemoryTracker {
public:
    struct Stats {
        size_t totalAllocated = 0;
        size_t totalFreed = 0;
        size_t currentUsage = 0;
        size_t peakUsage = 0;
        size_t allocationCount = 0;
        size_t deallocationCount = 0;
    };

    static MemoryTracker& instance() {
        static MemoryTracker tracker;
        return tracker;
    }

    void recordAllocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.totalAllocated += size;
        stats_.currentUsage += size;
        stats_.allocationCount++;
        if (stats_.currentUsage > stats_.peakUsage) {
            stats_.peakUsage = stats_.currentUsage;
        }
    }

    void recordDeallocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.totalFreed += size;
        if (stats_.currentUsage >= size) {
            stats_.currentUsage -= size;
        } else {
            stats_.currentUsage = 0;
        }
        stats_.deallocationCount++;
    }

    Stats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_ = Stats{};
    }

    size_t getPerEntityOverhead(size_t entityCount) const {
        auto stats = getStats();
        if (entityCount == 0) return 0;
        return stats.peakUsage / entityCount;
    }

private:
    MemoryTracker() = default;
    mutable std::mutex mutex_;
    Stats stats_;
};

} // namespace benchmark

// Global new/delete overrides for memory tracking
void* operator new(std::size_t size);
void operator delete(void* ptr, std::size_t size) noexcept;
