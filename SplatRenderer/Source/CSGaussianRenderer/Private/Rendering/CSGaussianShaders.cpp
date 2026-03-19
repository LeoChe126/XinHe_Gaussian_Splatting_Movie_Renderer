#include "CSGaussianShaders.h"

namespace CSGaussian
{

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianDistanceCS,
    "/Plugin/CSGaussianRenderer/Private/CSComputeDistance.usf",
    "main",
    SF_Compute);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianTransformCS,
    "/Plugin/CSGaussianRenderer/Private/CSComputeTransform.usf",
    "main",
    SF_Compute);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianIndirectArgsCS,
    "/Plugin/CSGaussianRenderer/Private/CSWriteIndirectArgs.usf",
    "main",
    SF_Compute);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianRenderVS,
    "/Plugin/CSGaussianRenderer/Private/CSRenderSplatVS.usf",
    "main",
    SF_Vertex);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianRenderPS,
    "/Plugin/CSGaussianRenderer/Private/CSRenderSplatPS.usf",
    "main",
    SF_Pixel);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianParticleFXCS,
    "/Plugin/CSGaussianRenderer/Private/CSComputeParticleFX.usf",
    "main",
    SF_Compute);

} // namespace CSGaussian
