#pragma once

#include "CSGaussianBuffers.h"
#include "PrimitiveSceneProxy.h"

class UCSGaussianComponent;

namespace CSGaussian
{

class FCSGaussianSceneProxy final : public FPrimitiveSceneProxy
{
public:
    FCSGaussianSceneProxy(UCSGaussianComponent& Component);

    virtual void CreateRenderThreadResources(FRHICommandListBase& RHICmdList) override;
    virtual void DestroyRenderThreadResources() override;
    virtual uint32 GetMemoryFootprint() const override { return sizeof(*this); }
    virtual SIZE_T GetTypeHash() const override
    {
        static size_t UniquePointer;
        return reinterpret_cast<SIZE_T>(&UniquePointer);
    }

    uint32 GetNumSplats() const { return NumSplats; }
    uint32 GetTextureWidth() const { return TextureWidth; }
    bool IsVisible(const FSceneView& View) const;

    FTextureRHIRef GetPositionTextureRHI() const { return PositionTextureRHI; }
    FTextureRHIRef GetCov1TextureRHI() const { return Cov1TextureRHI; }
    FTextureRHIRef GetCov2OpacityTextureRHI() const { return Cov2OpacityTextureRHI; }
    FTextureRHIRef GetRotationTextureRHI() const { return RotationTextureRHI; }
    FTextureRHIRef GetScaleOpacityTextureRHI() const { return ScaleOpacityTextureRHI; }
    FTextureRHIRef GetSH0TextureRHI() const { return SH0TextureRHI; }
    bool UsePrecomputedCov() const { return bUsePrecomputedCov; }
    float GetBrightness() const { return Brightness; }
    float GetSplatScale() const { return SplatScale; }
    void SetSplatScale(float InScale) { SplatScale = InScale; }
    bool IsCropEnabled() const { return bEnableCrop; }
    FVector3f GetCropCenter() const { return CropCenter; }
    FVector3f GetCropHalfSize() const { return CropHalfSize; }
    FMatrix44f GetCropInvRotation() const { return FMatrix44f(CropInvRotation); }

    FShaderResourceViewRHIRef GetIndicesSRV() const { return Indices.ShaderResourceViewRHI; }
    FUnorderedAccessViewRHIRef GetIndicesUAV() const { return Indices.UnorderedAccessViewRHI; }
    FShaderResourceViewRHIRef GetTransformsSRV() const { return Transforms.ShaderResourceViewRHI; }
    FUnorderedAccessViewRHIRef GetTransformsUAV() const { return Transforms.UnorderedAccessViewRHI; }
    FShaderResourceViewRHIRef GetVisibleCountSRV() const { return VisibleCount.ShaderResourceViewRHI; }
    FUnorderedAccessViewRHIRef GetVisibleCountUAV() const { return VisibleCount.UnorderedAccessViewRHI; }
    FBufferRHIRef GetIndirectArgsBuffer() const { return IndirectArgsBuffer; }

    FRDGBufferRef& GetIndicesFake() { return IndicesFake; }
    FRDGBufferRef& GetDistancesFake() { return DistancesFake; }

    FString GetName() const { return Name; }

    void SetBrightness(float InBrightness) { Brightness = InBrightness; }
    void SetCrop(bool bEnable, const FVector3f& InCenter, const FVector3f& InHalfSize, const FMatrix& InInvRotation)
    {
        if (bEnableCrop != bEnable || CropCenter != InCenter || CropHalfSize != InHalfSize)
            bSortCacheValid = false;
        bEnableCrop = bEnable;
        CropCenter = InCenter;
        CropHalfSize = InHalfSize;
        CropInvRotation = InInvRotation;
    }

    bool IsSortCacheValid(const FMatrix44f& CurrentLocalToClip) const
    {
        if (!bSortCacheValid) return false;
        const float* A = &CachedLocalToClip.M[0][0];
        const float* B = &CurrentLocalToClip.M[0][0];
        for (int32 i = 0; i < 16; ++i)
        {
            if (FMath::Abs(A[i] - B[i]) > 1e-5f)
                return false;
        }
        return true;
    }
    void SetSortCache(const FMatrix44f& LocalToClip) { CachedLocalToClip = LocalToClip; bSortCacheValid = true; }
    void InvalidateSortCache() { bSortCacheValid = false; }

    void UpdateTextures_RenderThread(
        FTextureRHIRef InPosition,
        FTextureRHIRef InCov1,
        FTextureRHIRef InCov2Opacity,
        FTextureRHIRef InRotation,
        FTextureRHIRef InScaleOpacity,
        FTextureRHIRef InSH0,
        uint32 InNumSplats,
        uint32 InTextureWidth,
        bool bInUsePrecomputedCov);

private:
    FTextureRHIRef PositionTextureRHI;
    FTextureRHIRef Cov1TextureRHI;
    FTextureRHIRef Cov2OpacityTextureRHI;
    FTextureRHIRef RotationTextureRHI;
    FTextureRHIRef ScaleOpacityTextureRHI;
    FTextureRHIRef SH0TextureRHI;
    bool bUsePrecomputedCov = true;
    float Brightness = 1.0f;
    float SplatScale = 1.0f;
    bool bEnableCrop = false;
    FVector3f CropCenter = FVector3f::ZeroVector;
    FVector3f CropHalfSize = FVector3f(1000, 1000, 1000);
    FMatrix CropInvRotation = FMatrix::Identity;

    uint32 NumSplats = 0;
    uint32 TextureWidth = 0;

    FCSGaussianGPUBuffer Indices;
    FCSGaussianGPUBuffer Transforms;
    FCSGaussianGPUBuffer VisibleCount;
    FBufferRHIRef IndirectArgsBuffer;

    FRDGBufferRef IndicesFake;
    FRDGBufferRef DistancesFake;

    FString Name;

    FMatrix44f CachedLocalToClip;
    bool bSortCacheValid = false;

    uint32 MaxSplats = 0;
};

} // namespace CSGaussian
