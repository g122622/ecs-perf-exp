#include "memory_tracker.hpp"

namespace benchmark {
// MemoryTracker implementation is header-only
// The global new/delete overrides are implemented here
} // namespace benchmark

// Global new/delete overrides
void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    if (ptr) {
        benchmark::MemoryTracker::instance().recordAllocation(size);
    }
    return ptr;
}

void operator delete(void* ptr, std::size_t size) noexcept {
    if (ptr) {
        benchmark::MemoryTracker::instance().recordDeallocation(size);
        std::free(ptr);
    }
}
