// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "TempoSceneCaptureComponent2D.generated.h"


UENUM(BlueprintType)
enum class ECaptureType : uint8 {
	NONE      UMETA(DisplayName = "None"),
	COLOR     UMETA(DisplayName = "Color"),
	DEPTH     UMETA(DisplayName = "Depth"),
	LABEL     UMETA(DisplayName = "Label"),
};


UCLASS()
class ROBOTBT_API UTempoSceneCaptureComponent2D : public USceneCaptureComponent2D {
	GENERATED_BODY()

public:
	UTempoSceneCaptureComponent2D();

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void StartCapture(ECaptureType NewCaptureType);

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void StopCapture();

	// The controller change this variable. We need this because we want to capture just when is executing a task
	UPROPERTY()
	bool CanCaptureNow = false;

protected:
	virtual void BeginPlay() override;

private:
	FString SaveDirectory;

	// Used by Capture and Save, to incriment the variable
	int32 ImageCounter;

	void CaptureAndSave();

	FTimerHandle CaptureTimerHandle;

	bool SaveAsJpeg(const FString& Filename, const TArray<FColor>& Bitmap, int32 Width, int32 Height);

	// The minimum depth this camera can measure (if depth is enabled). Will be set to the global near clip plane.
	UPROPERTY(EditAnywhere, Category = "Depth")
	float MinDepth = 10.0; // 10cm

	// The maximum depth this camera can measure (if depth is enabled). Will be set to UTempoSensorsSettings::MaxCameraDepth.
	UPROPERTY(EditAnywhere, Category = "Depth")
	float MaxDepth = 100000.0; // 1km

	// The maximum depth this camera can measure (if depth is enabled). Will be set to UTempoSensorsSettings::MaxCameraDepth.
	UPROPERTY(EditAnywhere, Category = "Depth")
	float kMaxDiscreteDepth = 2000;

	UPROPERTY(EditAnywhere, Category = "Depth")
	UMaterialInterface* BasePostProcessMaterial_NoDepth = nullptr;

	UPROPERTY(EditAnywhere, Category = "Depth")
	UMaterialInterface* BasePostProcessMaterial_WithDepth = nullptr;

	UPROPERTY(EditAnywhere, Category = "Depth")
	UMaterialInterface* BasePostNorm16ToFloat = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* CameraPostProcess = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void ApplyDepthFilter();

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void ApplyColorFilter();

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void ApplyLabelFilter();

};

