#include "renderConfig.hpp"


namespace siren::core
{

bool CameraInfo::operator==(const CameraInfo& o) const
{
    return projectionViewMatrix == o.projectionViewMatrix && position == o.position;
}

bool LightInfo::operator==(const LightInfo& o) const
{
    if (
        pointLightCount != o.pointLightCount ||
        directionalLightCount != o.directionalLightCount ||
        spotLightCount != o.spotLightCount
    ) {
        return false;
    }

    return pointLights == o.pointLights &&
            directionalLights == o.directionalLights &&
            spotLights == o.spotLights;
}

bool RenderInfo::operator==(const RenderInfo& o) const
{
    return cameraInfo == o.cameraInfo && lightInfo == o.lightInfo;
}

}
