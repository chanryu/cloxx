#pragma once

#include <string>

namespace cloxx {

class LoxObject {
public:
#ifdef CLOXX_GC_DEBUG
    LoxObject();
    virtual ~LoxObject();

    static size_t instanceCount();
#else
    virtual ~LoxObject() = default;
#endif

    virtual std::string toString() const = 0;
    virtual bool isTruthy() const;
    virtual bool equals(LoxObject const& object) const;
};

class LoxNil : public LoxObject {
public:
    std::string toString() const override;
    bool isTruthy() const override;
    bool equals(LoxObject const& object) const override;
};

class LoxNumber : public LoxObject {
public:
    explicit LoxNumber(double value);

    std::string toString() const override;
    bool equals(LoxObject const& object) const override;

    double const value;
};

class LoxString : public LoxObject {
public:
    explicit LoxString(std::string value);

    std::string toString() const override;
    bool equals(LoxObject const& object) const override;

    std::string const value;
};

class LoxBoolean : public LoxObject {
public:
    explicit LoxBoolean(bool value);

    std::string toString() const override;
    bool isTruthy() const override;
    bool equals(LoxObject const& object) const override;

    bool const value;
};

// Helper functions.

inline auto makeLoxNil()
{
    return std::make_shared<LoxNil>();
}

inline auto toLoxNumber(double value)
{
    return std::make_shared<LoxNumber>(value);
}

inline auto toLoxString(std::string value)
{
    return std::make_shared<LoxString>(std::move(value));
}

inline auto toLoxBoolean(bool value)
{
    return std::make_shared<LoxBoolean>(value);
}

} // namespace cloxx
