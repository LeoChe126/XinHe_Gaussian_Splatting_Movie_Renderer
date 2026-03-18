#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include "CSGaussianComponent.h"
#include "CSCropBoxComponent.h"
#include "CSGaussianActor.generated.h"

UCLASS(HideCategories = (Input, HLOD, LOD, Cooking, DataLayers, Replication, WorldPartition, Networking),
    meta = (DisplayName = "CS Gaussian Actor"))
class CSGAUSSIANRENDERER_API ACSGaussianActor : public AActor
{
    GENERATED_BODY()

public:
    ACSGaussianActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Data",
        meta = (FilePathFilter = "PLY files (*.ply)|*.ply"))
    FFilePath PLYFilePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Data")
    FFilePath AudioPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Data",
        meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float AudioVolume = 1.0f;

    UFUNCTION(BlueprintCallable, Category = "GS Data")
    bool LoadPLY(const FString& FilePath);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Rendering",
        meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "5.0"))
    float Brightness = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Rendering",
        meta = (ClampMin = "0.0", UIMin = "0.01", UIMax = "5.0"))
    float SplatScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Crop Volume")
    bool bEnableCrop = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Crop Volume",
        meta = (EditCondition = "bEnableCrop"))
    FVector CropCenter = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GS Crop Volume",
        meta = (EditCondition = "bEnableCrop", ClampMin = "0.0", UIMin = "0.0"))
    FVector CropSize = FVector(2000, 2000, 2000);

    UFUNCTION(BlueprintCallable, Category = "GS Rendering")
    void SetBrightness(float InBrightness);

    UFUNCTION(BlueprintCallable, Category = "GS Rendering")
    void SetSplatScale(float InScale);

    UFUNCTION(BlueprintCallable, Category = "GS Crop Volume")
    void SetCropVolume(bool bEnable, FVector InCenter, FVector InSize, FQuat InRotation);

    UFUNCTION(BlueprintCallable, Category = "GS Audio")
    void SetAudioVolume(float InVolume);

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCSGaussianComponent> GaussianComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCSCropBoxComponent> CropBoxComponent;

    virtual void Tick(float DeltaSeconds) override;
    virtual bool ShouldTickIfViewportsOnly() const override { return true; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void SyncBoxFromCropParams();
    void SyncCropToComponent();
    void OnCropBoxTransformUpdated(USceneComponent* Comp, EUpdateTransformFlags Flags, ETeleportType Teleport);
    void OnCropBoxExtentChanged();
    bool LoadAudioFromFile(const FString& FilePath);

    UPROPERTY(Transient)
    TArray<UTexture2D*> LoadedTextures;

    UPROPERTY()
    TObjectPtr<UAudioComponent> AudioComponent;

    UPROPERTY()
    TObjectPtr<USoundWaveProcedural> LoadedSoundWave;

    TArray<uint8> AudioPCMData;
    float AudioDuration = 0.0f;
    bool bUpdatingCropBox = false;
};
