#pragma once

#include "CoreMinimal.h"
#include "RobotProperties.h"
#include "RobotBT/Enum/FailureReasonEnum.h"
#include "ExperimentResult.generated.h"

USTRUCT(BlueprintType)
struct FTaskResult {
	GENERATED_BODY()

	FString Location = "";
	bool SuccessResult = false;
	EFailureReasonEnum FailureReasonEnum = EFailureReasonEnum::None;
	FString SkillUsed = "";
	float SkillChanceToFail = 0;
	float SkillBatteryConsumeDischargeRate = 0;
	// How much time spent to performe a specific task
	float TimeSpentOnTask = 0.0f;
	// Battery spent on this task
	float BatterySpentOnTask = 0.0f;
	FRobotProperties InitialRobotsProperties;
	FRobotProperties EndRobotsProperties;
};

USTRUCT(BlueprintType)
struct FExperimentResult{
	GENERATED_BODY()

	int32 ExperimentId; // preenchido por BaseExperiment
	FString Approach = "";
	float ExperimentTime = 0; // preenchido por BaseExperiment
	TArray<FTaskResult> TaskResults;
};