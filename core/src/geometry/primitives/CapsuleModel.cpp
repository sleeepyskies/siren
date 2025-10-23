#include "CapsuleModel.hpp"

namespace siren::geometry
{

CapsuleModel::CapsuleModel(const float radius, const float height)
    : PrimitiveModel() : m_height(height), m_radius(radius)
{
    NotImplemented;
}

} // namespace siren::geometry