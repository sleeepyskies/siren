#pragma once

#include "Primitive.hpp"

namespace siren::geometry
{

class PlaneModel final : public PrimitiveModel
{
public:
    struct PlaneModelParams {
        float width            = 2;
        float depth            = 2;
        uint32_t subdivisionsX = 0;
        uint32_t subdivisionsY = 0;
    };

    PRIMITIVE_TYPE(PrimitiveType::PLANE);
    ~PlaneModel() override = default;

    PlaneModelParams getParams() const;
    void setParams(PlaneModelParams params);

private:
    explicit PlaneModel(const PlaneModelParams& params);
    explicit PlaneModel();

    PlaneModelParams m_params;

    void regenerate() override;

    friend class AssetManager;
};

} // namespace siren::geometry
