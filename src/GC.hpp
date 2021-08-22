#pragma once

#include <memory>
#include <vector>

namespace cloxx {

class GarbageCollector;

class Traceable {
public:
    Traceable();
    virtual ~Traceable();

    static int instanceCount();

    struct Enumerator {
        virtual void enumerate(Traceable& traceable) const = 0;
    };

    virtual void enumTraceables(Enumerator const& enumerator) = 0;
    virtual void reclaimTraceables() = 0;

private:
    friend class GarbageCollector;
    bool _isReachable;

    static int _instanceCount;
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
