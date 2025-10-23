#pragma once

#include "geometry/Model.hpp"

namespace siren::geometry
{

enum class PrimitiveType {
    PLANE, //
    CUBE,
    CAPSULE
};

#define PRIMITIVE_TYPE(type)                                                                       \
    geometry::PrimitiveType getPrimitiveType() const override                                      \
    {                                                                                              \
        return type;                                                                               \
    }                                                                                              \
    static geometry::PrimitiveType getStaticPrimitiveType()                                        \
    {                                                                                              \
        return type;                                                                               \
    }

class PrimitiveModel : public Model
{
public:
    /// @brief Handles default naming all primitive models
    explicit PrimitiveModel(const PrimitiveType type);
    ~PrimitiveModel() override = 0;

    virtual PrimitiveType getPrimitiveType() const = 0;

private:
    virtual void regenerate() = 0;
};

} // namespace siren::geometry