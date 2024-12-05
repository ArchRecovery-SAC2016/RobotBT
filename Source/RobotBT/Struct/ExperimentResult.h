#pragma once

#include "CoreMinimal.h"
#include "RobotProperties.h"
#include "RobotBT/Enum/FailureReasonEnum.h"
#include "ExperimentResult.generated.h"

USTRUCT(BlueprintType)
struct FTaskResult {
	GENERATED_BODY()

	UPROPERTY()
	FString Location = "";

	UPROPERTY()
	bool SuccessResult = false;

	UPROPERTY()
	EFailureReasonEnum FailureReasonEnum = EFailureReasonEnum::None;

	UPROPERTY()
	FString SkillUsed = "";

	UPROPERTY()
	float SkillChanceToFail = 0;

	UPROPERTY()
	float SkillBatteryConsumeDischargeRate = 0;

	// How much time spent to performe a specific task
	UPROPERTY()
	float TimeSpentOnTask = 0.0f;

	// Battery spent on this task
	UPROPERTY()
	float BatterySpentOnTask = 0.0f;

	// TODO: REMMOVER ISSO AKI!!!
	UPROPERTY()
	FRobotProperties InitialRobotsProperties;

	UPROPERTY()
	FRobotProperties EndRobotsProperties;
};

USTRUCT(BlueprintType)
struct FExperimentResult{
	GENERATED_BODY()

	UPROPERTY()
	int32 ExperimentId; // preenchido por BaseExperiment

	UPROPERTY()
	FString Approach = "";

	UPROPERTY()
	float WallClockInSeconds = 0; // The experiment timer

	UPROPERTY()
	TArray<FTaskResult> TaskResults;

	UPROPERTY()
	TArray<FRobotProperties> Robots;
};