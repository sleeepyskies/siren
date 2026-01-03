#include "GPULight.hpp"

#include <cstring>
#include <tuple>


namespace siren::core
{
bool GPUPointLight::operator==(const GPUPointLight& o) const
{
    return std::tie(p1, p2, p3, c1, c2, c3) == std::tie(o.p1, o.p2, o.p3, o.c1, o.c2, o.c3);
}

bool GPUDirectionalLight::operator==(const GPUDirectionalLight& o) const
{
    return std::tie(d1, d2, d3, c1, c2, c3) == std::tie(o.d1, o.d2, o.d3, o.c1, o.c2, o.c3);
}

bool GPUSpotLight::operator==(const GPUSpotLight& o) const
{
    // safe because we have fully packed the struct, no padding.
    return std::memcmp(this, &o, sizeof(GPUSpotLight)) == 0;
}
}
