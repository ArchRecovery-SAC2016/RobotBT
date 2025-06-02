// Copyright Tempo Simulation, LLC. All Rights Reserved

#pragma once

#include "TempoSensorInterface.h"

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"

#include "TempoSceneCaptureComponent2D.generated.h"

UCLASS()
class ROBOTBT_API UTempoSceneCaptureComponent2D : public USceneCaptureComponent2D {
	GENERATED_BODY()

public:
	UTempoSceneCaptureComponent2D();

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
};

