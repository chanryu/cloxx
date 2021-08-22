#pragma once

#include <memory>
#include <vector>

namespace cloxx {

class GarbageCollector;

class Traceable {
public:
#ifndef NDEBUG
    Traceable();
    virtual ~Traceable();
    static size_t instanceCount();
#else
    virtual ~Traceable() = default;
#endif

    struct Enumerator {
        virtual void enumerate(Traceable& traceable) const = 0;
    };

    virtual void enumTraceables(Enumerator const& enumerator) = 0;
    virtual void reclaimTraceables() = 0;

private:
    friend class GarbageCollector;
    bool _isReachable;
};

class GarbageCollector {
public:
    explicit GarbageCollector(std::shared_ptr<Traceable> const& root);
    ~GarbageCollector();

    void addTraceable(std::shared_ptr<Traceable> const& traceable);

    size_t countTraceables() const;

    void collectGarbage();

private:
    std::shared_ptr<Traceable> _root;
    std::vector<std::weak_ptr<Traceable>> _weakTraceables;
};

} // namespace cloxx
