#include "GC.hpp"

#include "Assert.hpp"

namespace cloxx {

#ifndef NDEBUG
namespace {
size_t traceableInstanceCount = 0;
}

Traceable::Traceable()
{
    traceableInstanceCount++;
}

Traceable::~Traceable()
{
    traceableInstanceCount--;
}

size_t Traceable::instanceCount()
{
    return traceableInstanceCount;
}
#endif

GarbageCollector::GarbageCollector(std::shared_ptr<Traceable> const& root) : _root{root}
{
    LOX_ASSERT(_root);
}

GarbageCollector::~GarbageCollector()
{
    _root->reclaimTraceables();
    collectGarbage();
}

void GarbageCollector::addTraceable(std::shared_ptr<Traceable> const& traceable)
{
    _weakTraceables.push_back(traceable);
}

size_t GarbageCollector::countTraceables() const
{
    size_t count = 0;
    for (auto& traceable : _weakTraceables) {
        if (auto t = traceable.lock()) {
            count += 1;
        }
    }
    return count;
}

void GarbageCollector::collectGarbage()
{
    // prologue: collect all traceables
    std::vector<std::shared_ptr<Traceable>> traceables;
    for (auto& weakTraceable : _weakTraceables) {
        if (auto traceable = weakTraceable.lock()) {
            traceables.push_back(traceable);
        }
    }

    // Mark & Sweep - Step 1: unmark all reachable traceables
    for (auto& traceable : traceables) {
        traceable->_isReachable = false;
    }

    // Mark & Sweep - Step 2: mark all reachable traceables
    struct Marker : Traceable::Enumerator {
        void enumerate(Traceable& traceable) const override
        {
            if (!traceable._isReachable) {
                traceable._isReachable = true;
                traceable.enumTraceables(*this);
            }
        }
    };
    _root->enumTraceables(Marker{});

    // Mark & Sweep - Step 3: reclaimTraceables unreachable traceables
    for (auto& traceable : traceables) {
        if (!traceable->_isReachable) {
            traceable->reclaimTraceables();
            traceable.reset();
        }
    }

    // epilogue: rearrange _weakTraceables
    _weakTraceables.clear();
    for (auto& traceable : traceables) {
        if (traceable) {
            _weakTraceables.push_back(traceable);
        }
    }
}

} // namespace cloxx
