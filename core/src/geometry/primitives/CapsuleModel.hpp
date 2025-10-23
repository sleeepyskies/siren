#pragma once

#include "Primitive.hpp"

namespace siren::geometry
{

class CapsuleModel final : public PrimitiveModel
{
public:
    explicit CapsuleModel(float radius, float height);
    ~CapsuleModel() override = default;
    PRIMITIVE_TYPE(PrimitiveType::CAPSULE);

private:
    float m_height;
    float m_radius;
};

} // namespace siren::geometry
