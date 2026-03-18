#include "CSGaussianSceneProxy.h"
#include "CSGaussianComponent.h"
#include "CSGaussianSubsystem.h"

namespace CSGaussian
{

FCSGaussianSceneProxy::FCSGaussianSceneProxy(UCSGaussianComponent& Component)
    : FPrimitiveSceneProxy(&Component)
{
    NumSplats = Component.GetGaussianCount();
    TextureWidth = Component.GetTextureWidth();
    MaxSplats = 0;

    auto GetTextureRHI = [](UTexture2D* Tex) -> FTextureRHIRef
    {
        if (Tex && Tex->GetResource())
            return Tex->GetResource()->TextureRHI;
        return nullptr;
    };

    bUsePrecomputedCov = Component.bUsePrecomputedCov;
    Brightness = Component.Brightness;
    bEnableCrop = Component.bEnableCrop;
    CropCenter = FVector3f(Component.CropCenter);
    CropHalfSize = FVector3f(Component.CropHalfSize);
    CropInvRotation = FRotationMatrix(Component.CropRotation.Inverse().Rotator());
    PositionTextureRHI = GetTextureRHI(Component.PositionTexture);
    Cov1TextureRHI = GetTextureRHI(Component.Cov1Texture);
    Cov2OpacityTextureRHI = GetTextureRHI(Component.Cov2OpacityTexture);
    RotationTextureRHI = GetTextureRHI(Component.RotationTexture);
    ScaleOpacityTextureRHI = GetTextureRHI(Component.ScaleOpacityTexture);
    SH0TextureRHI = GetTextureRHI(Component.SH0Texture);
    SplatScale = Component.SplatScale;

    Name = Component.GetOwner() ? Component.GetOwner()->GetName() : TEXT("Unknown");
}

void FCSGaussianSceneProxy::CreateRenderThreadResources(FRHICommandListBase& RHICmdList)
{
    if (NumSplats > 0 && MaxSplats == 0)
    {
        MaxSplats = NumSplats;
        Indices = FCSGaussianGPUBuffer(MaxSplats, PF_R32_UINT);
        Transforms = FCSGaussianGPUBuffer(MaxSplats, PF_FloatRGBA);
        VisibleCount = FCSGaussianGPUBuffer(1, PF_R32_UINT);
        Indices.InitRHI(RHICmdList);
        Transforms.InitRHI(RHICmdList);
        VisibleCount.InitRHI(RHICmdList);

        FRHIResourceCreateInfo ArgsCreateInfo(TEXT("CSIndirectArgs"));
        IndirectArgsBuffer = RHICmdList.CreateBuffer(
            sizeof(uint32) * 4,
            EBufferUsageFlags::DrawIndirect | EBufferUsageFlags::UnorderedAccess,
            sizeof(uint32),
            ERHIAccess::IndirectArgs,
            ArgsCreateInfo);
    }

    if (GEngine)
    {
        UCSGaussianSubsystem* Subsystem = GEngine->GetEngineSubsystem<UCSGaussianSubsystem>();
        if (Subsystem)
        {
            Subsystem->AddGaussianProxy_RenderThread(this);
        }
    }
}

void FCSGaussianSceneProxy::DestroyRenderThreadResources()
{
    if (GEngine)
    {
        UCSGaussianSubsystem* Subsystem = GEngine->GetEngineSubsystem<UCSGaussianSubsystem>();
        if (Subsystem)
        {
            Subsystem->RemoveGaussianProxy_RenderThread(this);
        }
    }

    Indices.ReleaseResource();
    Transforms.ReleaseResource();
    VisibleCount.ReleaseResource();
    IndirectArgsBuffer.SafeRelease();
}

bool FCSGaussianSceneProxy::IsVisible(const FSceneView& View) const
{
    if (NumSplats == 0 || !PositionTextureRHI || !SH0TextureRHI ||
        !Indices.ShaderResourceViewRHI || !Transforms.ShaderResourceViewRHI)
        return false;

    if (bUsePrecomputedCov)
    {
        if (!Cov1TextureRHI || !Cov2OpacityTextureRHI)
            return false;
    }
    else
    {
        if (!RotationTextureRHI || !ScaleOpacityTextureRHI)
            return false;
    }

    if (!IsShown(&View) || &GetScene() != View.Family->Scene)
        return false;

    FPrimitiveComponentId PrimId = GetPrimitiveComponentId();
    if (View.HiddenPrimitives.Contains(PrimId))
        return false;
    if (View.ShowOnlyPrimitives.IsSet() && !View.ShowOnlyPrimitives->Contains(PrimId))
        return false;

    return true;
}

void FCSGaussianSceneProxy::UpdateTextures_RenderThread(
    FTextureRHIRef InPosition,
    FTextureRHIRef InCov1,
    FTextureRHIRef InCov2Opacity,
    FTextureRHIRef InRotation,
    FTextureRHIRef InScaleOpacity,
    FTextureRHIRef InSH0,
    uint32 InNumSplats,
    uint32 InTextureWidth,
    bool bInUsePrecomputedCov)
{
    check(IsInRenderingThread());

    bSortCacheValid = false;
    bUsePrecomputedCov = bInUsePrecomputedCov;
    PositionTextureRHI = InPosition;
    Cov1TextureRHI = InCov1;
    Cov2OpacityTextureRHI = InCov2Opacity;
    RotationTextureRHI = InRotation;
    ScaleOpacityTextureRHI = InScaleOpacity;
    SH0TextureRHI = InSH0;
    NumSplats = InNumSplats;
    TextureWidth = InTextureWidth;

    if (InNumSplats > MaxSplats)
    {
        Indices.ReleaseResource();
        Transforms.ReleaseResource();
        VisibleCount.ReleaseResource();
        IndirectArgsBuffer.SafeRelease();

        MaxSplats = InNumSplats;
        Indices = FCSGaussianGPUBuffer(MaxSplats, PF_R32_UINT);
        Transforms = FCSGaussianGPUBuffer(MaxSplats, PF_FloatRGBA);
        VisibleCount = FCSGaussianGPUBuffer(1, PF_R32_UINT);

        FRHICommandListImmediate& RHICmdList = FRHICommandListImmediate::Get();
        Indices.InitRHI(RHICmdList);
        Transforms.InitRHI(RHICmdList);
        VisibleCount.InitRHI(RHICmdList);

        FRHIResourceCreateInfo ArgsCreateInfo(TEXT("CSIndirectArgs"));
        IndirectArgsBuffer = RHICmdList.CreateBuffer(
            sizeof(uint32) * 4,
            EBufferUsageFlags::DrawIndirect | EBufferUsageFlags::UnorderedAccess,
            sizeof(uint32),
            ERHIAccess::IndirectArgs,
            ArgsCreateInfo);
    }
}

} // namespace CSGaussian
