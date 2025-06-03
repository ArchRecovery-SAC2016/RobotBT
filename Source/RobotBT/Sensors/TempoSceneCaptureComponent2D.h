// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoSensorInterface.h"

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"

#include "TempoSceneCaptureComponent2D.generated.h"

struct FCameraPixelNoDepth {
	static constexpr bool bSupportsDepth = false;

	uint8 B() const { return U1; }
	uint8 G() const { return U2; }
	uint8 R() const { return U3; }

	uint8 Label() const { return U4; }

private:
	uint8 U1 = 0;
	uint8 U2 = 0;
	uint8 U3 = 0;
	uint8 U4 = 0;
};

// 8-byte pixel format where first 3 bytes are color, 4th byte is label.
// 5th-8th bytes are a uint32 representing discrete depth.
struct FCameraPixelWithDepth {
	static constexpr bool bSupportsDepth = true;

	uint8 B() const { return U3; }
	uint8 G() const { return U2; }
	uint8 R() const { return U1; }

	uint8 Label() const { return U4; }

	float Depth(float MinDepth, float MaxDepth, float MaxDiscretizedDepth) const {
		// We discretize inverse depth to give more consistent precision vs depth.
		// See https://developer.nvidia.com/content/depth-precision-visualized
		const float InverseDepthFraction = static_cast<float>(U5) / MaxDiscretizedDepth;
		const float InverseDepth = InverseDepthFraction * (1.0 / MinDepth - 1.0 / MaxDepth) + 1.0 / MaxDepth;
		return 1.0 / InverseDepth;
	}

private:
	uint8 U1 = 0;
	uint8 U2 = 0;
	uint8 U3 = 0;
	uint8 U4 = 0;
	uint32 U5 = 0;
};


UCLASS()
class ROBOTBT_API UTempoSceneCaptureComponent2D : public USceneCaptureComponent2D {
	GENERATED_BODY()

public:
	UTempoSceneCaptureComponent2D();
	void ApplyDepthEnabled();

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void StartCapture();

	UFUNCTION(BlueprintCallable, Category = "Capture")
	void StopCapture();

protected:
	virtual void BeginPlay() override;

private:
	bool bIsCapturing;
	FString SaveDirectory;
	int32 ImageCounter;

	void CaptureAndSave();

	FTimerHandle CaptureTimerHandle;

	bool SaveAsJpeg(const FString& Filename, const TArray<FColor>& Bitmap, int32 Width, int32 Height);

	// Whether this camera can measure depth. Disabled when not requested to optimize performance.
	UPROPERTY(EditAnywhere, Category = "Depth")
	bool bDepthEnabled = false;

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


};

