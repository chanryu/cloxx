#pragma once

#include <memory>
#include <vector>

namespace cloxx {

class Environment;
class GarbageCollector;

class Traceable {
public:
    class PrivateCreationTag {
        // only GarbageCollector can create it
        friend class GarbageCollector;
        PrivateCreationTag(){};
    };

    Traceable(PrivateCreationTag);

#ifndef NDEBUG
    virtual ~Traceable();
    static size_t instanceCount();
#else
    virtual ~Traceable() = default;
#endif

    struct Enumerator {
        virtual void enumerate(Traceable& traceable) const = 0;
    };

    virtual void enumerateTraceables(Enumerator const& enumerator) = 0;
    virtual void reclaim() = 0;

private:
    friend class GarbageCollector;
    bool _isReachable;
};

class GarbageCollector {
public:
    GarbageCollector();
    ~GarbageCollector();

    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args)
    {
        auto traceable = std::make_shared<T>(Traceable::PrivateCreationTag{}, std::forward<Args>(args)...);
        _weakTraceables.push_back(traceable);
        return traceable;
    }

    std::shared_ptr<Environment> const& root();

    size_t collect();

private:
    std::shared_ptr<Environment> _root;
    std::vector<std::weak_ptr<Traceable>> _weakTraceables;
};

} // namespace cloxx
