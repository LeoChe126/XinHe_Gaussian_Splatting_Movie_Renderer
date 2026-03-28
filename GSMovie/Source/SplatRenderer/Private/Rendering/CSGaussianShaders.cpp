#include "CSGaussianShaders.h"

namespace CSGaussian
{

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianDistanceCS,
    "/Plugin/GSMovie/Private/CSComputeDistance.usf",
    "main",
    SF_Compute);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianTransformCS,
    "/Plugin/GSMovie/Private/CSComputeTransform.usf",
    "main",
    SF_Compute);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianIndirectArgsCS,
    "/Plugin/GSMovie/Private/CSWriteIndirectArgs.usf",
    "main",
    SF_Compute);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianRenderVS,
    "/Plugin/GSMovie/Private/CSRenderSplatVS.usf",
    "main",
    SF_Vertex);

IMPLEMENT_GLOBAL_SHADER(
    FCSGaussianRenderPS,
    "/Plugin/GSMovie/Private/CSRenderSplatPS.usf",
    "main",
    SF_Pixel);

// ParticleFX shader registration disabled for now (advanced feature, not in release)
// IMPLEMENT_GLOBAL_SHADER(
//     FCSGaussianParticleFXCS,
//     "/Plugin/GSMovie/Private/CSComputeParticleFX.usf",
//     "main",
//     SF_Compute);

} // namespace CSGaussian
