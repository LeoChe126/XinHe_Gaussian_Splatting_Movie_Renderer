#include "CSGaussianComponent.h"
#include "Rendering/CSGaussianSceneProxy.h"
#include "RenderingThread.h"

UCSGaussianComponent::UCSGaussianComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetCollisionEnabled(ECollisionEnabled::NoCollision);
    bCastDynamicShadow = false;
    bCastStaticShadow = false;
}

#if WITH_EDITOR
void UCSGaussianComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropName = PropertyChangedEvent.GetMemberPropertyName();

    if (PropName == GET_MEMBER_NAME_CHECKED(UCSGaussianComponent, Brightness))
    {
        UpdateProxyBrightness();
    }

    if (PropName == GET_MEMBER_NAME_CHECKED(UCSGaussianComponent, SplatScale))
    {
        SetSplatScale(SplatScale);
    }
}
#endif

void UCSGaussianComponent::OnUnregister()
{
    FlushRenderingCommands();
    Super::OnUnregister();
}

FPrimitiveSceneProxy* UCSGaussianComponent::CreateSceneProxy()
{
    return new CSGaussian::FCSGaussianSceneProxy(*this);
}

FBoxSphereBounds UCSGaussianComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    return FBoxSphereBounds(FVector::ZeroVector, FVector(100000.f), 100000.f).TransformBy(LocalToWorld);
}

uint32 UCSGaussianComponent::GetTextureWidth() const
{
    if (PositionTexture)
        return PositionTexture->GetSizeX();
    return 0;
}

void UCSGaussianComponent::SetGaussianTextures(
    UTexture2D* InPosition,
    UTexture2D* InCov1,
    UTexture2D* InCov2Opacity,
    UTexture2D* InSH0,
    int32 InGaussianCount)
{
    bUsePrecomputedCov = true;
    PositionTexture = InPosition;
    Cov1Texture = InCov1;
    Cov2OpacityTexture = InCov2Opacity;
    RotationTexture = nullptr;
    ScaleOpacityTexture = nullptr;
    SH0Texture = InSH0;
    GaussianCount = InGaussianCount;

    UpdateProxyTextures();
}

void UCSGaussianComponent::SetSequenceTextures(
    UTexture2D* InPosition,
    UTexture2D* InRotation,
    UTexture2D* InScaleOpacity,
    UTexture2D* InSH0,
    int32 InGaussianCount)
{
    bUsePrecomputedCov = false;
    PositionTexture = InPosition;
    Cov1Texture = nullptr;
    Cov2OpacityTexture = nullptr;
    RotationTexture = InRotation;
    ScaleOpacityTexture = InScaleOpacity;
    SH0Texture = InSH0;
    GaussianCount = InGaussianCount;

    UpdateProxyTextures();
}

void UCSGaussianComponent::UpdateProxyTextures()
{
    if (!SceneProxy)
    {
        MarkRenderStateDirty();
        return;
    }

    auto GetTextureRHI = [](UTexture2D* Tex) -> FTextureRHIRef
    {
        if (Tex && Tex->GetResource())
            return Tex->GetResource()->TextureRHI;
        return nullptr;
    };

    FTextureRHIRef PosRHI = GetTextureRHI(PositionTexture);
    FTextureRHIRef Cov1RHI = GetTextureRHI(Cov1Texture);
    FTextureRHIRef Cov2OpRHI = GetTextureRHI(Cov2OpacityTexture);
    FTextureRHIRef RotRHI = GetTextureRHI(RotationTexture);
    FTextureRHIRef ScaleOpRHI = GetTextureRHI(ScaleOpacityTexture);
    FTextureRHIRef SH0RHI = GetTextureRHI(SH0Texture);

    if (!PosRHI && PositionTexture)
        return;

    uint32 Count = (uint32)FMath::Max(GaussianCount, 0);
    uint32 TexWidth = GetTextureWidth();
    bool bPrecomputed = bUsePrecomputedCov;
    float BrightnessVal = Brightness;
    bool bCrop = bEnableCrop;
    FVector3f Center3f(CropCenter);
    FVector3f HalfSize3f(CropHalfSize);
    FMatrix InvRot = FRotationMatrix(CropRotation.Inverse().Rotator());
    float SScale = SplatScale;

    CSGaussian::FCSGaussianSceneProxy* Proxy =
        static_cast<CSGaussian::FCSGaussianSceneProxy*>(SceneProxy);

    ENQUEUE_RENDER_COMMAND(UpdateCSGaussianTextures)(
        [Proxy, PosRHI, Cov1RHI, Cov2OpRHI, RotRHI, ScaleOpRHI, SH0RHI, Count, TexWidth, bPrecomputed, BrightnessVal, bCrop, Center3f, HalfSize3f, InvRot, SScale]
        (FRHICommandListImmediate& RHICmdList)
        {
            Proxy->SetBrightness(BrightnessVal);
            Proxy->SetSplatScale(SScale);
            Proxy->SetCrop(bCrop, Center3f, HalfSize3f, InvRot);
            Proxy->UpdateTextures_RenderThread(
                PosRHI, Cov1RHI, Cov2OpRHI, RotRHI, ScaleOpRHI, SH0RHI,
                Count, TexWidth, bPrecomputed);
        });
}

void UCSGaussianComponent::SetBrightness(float InBrightness)
{
    Brightness = FMath::Max(InBrightness, 0.0f);
    UpdateProxyBrightness();
}

void UCSGaussianComponent::SetCropVolume(bool bEnable, FVector InCenter, FVector InHalfSize, FQuat InRotation)
{
    bEnableCrop = bEnable;
    CropCenter = InCenter;
    CropHalfSize = InHalfSize;
    CropRotation = InRotation;

    if (!SceneProxy) return;

    FVector3f Center3f(InCenter);
    FVector3f HalfSize3f(InHalfSize);
    FMatrix InvRot = FRotationMatrix(InRotation.Inverse().Rotator());
    CSGaussian::FCSGaussianSceneProxy* Proxy =
        static_cast<CSGaussian::FCSGaussianSceneProxy*>(SceneProxy);

    ENQUEUE_RENDER_COMMAND(UpdateCSGaussianCrop)(
        [Proxy, bEnable, Center3f, HalfSize3f, InvRot](FRHICommandListImmediate&)
        {
            Proxy->SetCrop(bEnable, Center3f, HalfSize3f, InvRot);
        });
}

void UCSGaussianComponent::UpdateProxyBrightness()
{
    if (!SceneProxy) return;

    float BrightnessVal = Brightness;
    CSGaussian::FCSGaussianSceneProxy* Proxy =
        static_cast<CSGaussian::FCSGaussianSceneProxy*>(SceneProxy);

    ENQUEUE_RENDER_COMMAND(UpdateCSGaussianBrightness)(
        [Proxy, BrightnessVal](FRHICommandListImmediate&)
        {
            Proxy->SetBrightness(BrightnessVal);
        });
}

void UCSGaussianComponent::SetSplatScale(float InScale)
{
    SplatScale = FMath::Max(InScale, 0.0f);

    if (!SceneProxy) return;

    float Scale = SplatScale;
    CSGaussian::FCSGaussianSceneProxy* Proxy =
        static_cast<CSGaussian::FCSGaussianSceneProxy*>(SceneProxy);

    ENQUEUE_RENDER_COMMAND(UpdateCSGaussianSplatScale)(
        [Proxy, Scale](FRHICommandListImmediate&)
        {
            Proxy->SetSplatScale(Scale);
        });
}
