#include "GC.hpp"

#include "Assert.hpp"
#include "Environment.hpp"

namespace cloxx {

#ifndef NDEBUG
namespace {
size_t traceableInstanceCount = 0;
}
#endif

Traceable::Traceable(PrivateCreationTag)
{
#ifndef NDEBUG
    traceableInstanceCount++;
#endif
}

#ifndef NDEBUG
Traceable::~Traceable()
{
    traceableInstanceCount--;
}

size_t Traceable::instanceCount()
{
    return traceableInstanceCount;
}
#endif

GarbageCollector::GarbageCollector()
{
    _root = create<Environment>();
}

GarbageCollector::~GarbageCollector()
{
    _root.reset();
    collect();
}

std::shared_ptr<Environment> const& GarbageCollector::root()
{
    return _root;
}

size_t GarbageCollector::collect()
{
    size_t collectedCount = 0;

    // Prologue: Collect all traceables
    std::vector<std::shared_ptr<Traceable>> traceables;
    for (auto& weakTraceable : _weakTraceables) {
        if (auto traceable = weakTraceable.lock()) {
            traceables.push_back(traceable);
        }
    }

    // Mark & Sweep - Step 1: Unmark all reachable traceables
    for (auto& traceable : traceables) {
        traceable->_isReachable = false;
    }

    // Mark & Sweep - Step 2: Mark all reachable traceables
    if (_root) {
        struct Marker : Traceable::Enumerator {
            void enumerate(Traceable& traceable) const override
            {
                if (!traceable._isReachable) {
                    traceable._isReachable = true;
                    traceable.enumerateTraceables(*this);
                }
            }
        };
        Marker{}.enumerate(*_root);
    }

    // Mark & Sweep - Step 3: Reclaim unreachable traceables
    for (auto& traceable : traceables) {
        if (!traceable->_isReachable) {
            traceable->reclaim();
            traceable.reset();
            collectedCount += 1;
        }
    }

    // Epilogue: Rearrange _weakTraceables
    _weakTraceables.clear();
    for (auto& traceable : traceables) {
        if (traceable) {
            _weakTraceables.push_back(traceable);
        }
    }

    return collectedCount;
}

} // namespace cloxx
