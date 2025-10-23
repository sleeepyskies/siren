#include "Primitive.hpp"

namespace siren::geometry
{

// todo: a more formal way of assigning names would be good. maybe reusing ids on delete?

static std::string typeToName(const PrimitiveType type)
{
    static size_t planeCount   = 0;
    static size_t cubeCount    = 0;
    static size_t capsuleCount = 0;

    switch (type) {
        case PrimitiveType::PLANE  : return "Plane_" + std::to_string(planeCount++);
        case PrimitiveType::CUBE   : return "Cube_" + std::to_string(cubeCount++);
        case PrimitiveType::CAPSULE: return "Capsule_" + std::to_string(capsuleCount++);
    }

    SirenAssert(false, "Invalid PrimitiveType encountered");
}

PrimitiveModel::PrimitiveModel(const PrimitiveType type) : Model(typeToName(type))
{
}

} // namespace siren::geometry