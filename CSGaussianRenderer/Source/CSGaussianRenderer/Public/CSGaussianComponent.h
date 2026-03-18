#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Texture2D.h"
#include "CSGaussianComponent.generated.h"

UCLASS(ClassGroup=(Rendering),
    HideCategories = (Rendering, Physics, Collision, Input, HLOD, Mobile, Navigation, VirtualTexture, ComponentTick, Tags, Cooking, LOD, ComponentReplication, Activation, Trigger, AssetUserData, Sockets),
    meta=(BlueprintSpawnableComponent, DisplayName="CS Gaussian Renderer"))
class CSGAUSSIANRENDERER_API UCSGaussianComponent : public UPrimitiveComponent
{
    GENERATED_BODY()

public:
    UCSGaussianComponent();

    UPROPERTY(Transient)
    UTexture2D* PositionTexture = nullptr;

    UPROPERTY(Transient)
    UTexture2D* Cov1Texture = nullptr;

    UPROPERTY(Transient)
    UTexture2D* Cov2OpacityTexture = nullptr;

    UPROPERTY(Transient)
    UTexture2D* RotationTexture = nullptr;

    UPROPERTY(Transient)
    UTexture2D* ScaleOpacityTexture = nullptr;

    UPROPERTY(Transient)
    UTexture2D* SH0Texture = nullptr;

    int32 GaussianCount = 0;

    bool bUsePrecomputedCov = true;

    UPROPERTY(BlueprintReadWrite, Category = "GS Rendering")
    float Brightness = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "GS Crop")
    bool bEnableCrop = false;

    FVector CropCenter = FVector::ZeroVector;
    FVector CropHalfSize = FVector(1000, 1000, 1000);
    FQuat CropRotation = FQuat::Identity;

    float SplatScale = 1.0f;

    UFUNCTION(BlueprintCallable, Category = "GS Rendering")
    void SetBrightness(float InBrightness);

    UFUNCTION(BlueprintCallable, Category = "GS Crop")
    void SetCropVolume(bool bEnable, FVector InCenter, FVector InHalfSize, FQuat InRotation);

    UFUNCTION(BlueprintCallable, Category = "GS Rendering")
    void SetSplatScale(float InScale);

    UFUNCTION(BlueprintCallable, Category = "Gaussian Data")
    void SetGaussianTextures(
        UTexture2D* InPosition,
        UTexture2D* InCov1,
        UTexture2D* InCov2Opacity,
        UTexture2D* InSH0,
        int32 InGaussianCount);

    UFUNCTION(BlueprintCallable, Category = "Gaussian Data")
    void SetSequenceTextures(
        UTexture2D* InPosition,
        UTexture2D* InRotation,
        UTexture2D* InScaleOpacity,
        UTexture2D* InSH0,
        int32 InGaussianCount);

    UFUNCTION(BlueprintCallable, Category = "Gaussian Data")
    int32 GetGaussianCount() const { return GaussianCount; }

    uint32 GetTextureWidth() const;

    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    virtual void OnUnregister() override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    void UpdateProxyBrightness();

private:
    void UpdateProxyTextures();
};
