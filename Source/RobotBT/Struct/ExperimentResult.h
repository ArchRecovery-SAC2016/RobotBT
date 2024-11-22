#pragma once

#include "CoreMinimal.h"
#include "RobotProperties.h"
#include "RobotBT/Enum/FailureReasonEnum.h"
#include "ExperimentResult.generated.h"

USTRUCT(BlueprintType)
struct FTaskResult {
	GENERATED_BODY()

	FString TaskName = "";
	FString Location = "";
	bool SuccessResult = false;
	EFailureReasonEnum FailureReasonEnum = EFailureReasonEnum::None;
	FRobotProperties InitialRobotsProperties;
	FRobotProperties EndRobotsProperties;
};

USTRUCT(BlueprintType)
struct FExperimentResult{
	GENERATED_BODY()

	int32 ExperimentId; // preenchido por BaseExperiment
	FString Approach = "Baseline";
	float ExperimentTime = 0; // preenchido por BaseExperiment
	TArray<FTaskResult> TaskResults;
};