#pragma once

#include "CoreMinimal.h"
#include "RobotProperties.h"
#include "RobotBT/Enum/FailureReasonEnum.h"
#include "ExperimentResult.generated.h"

USTRUCT(BlueprintType)
struct FTaskResult {
	GENERATED_BODY()

	UPROPERTY()
	FString RobotName = "";

	UPROPERTY()
	ESkillEnum TaskName;

	UPROPERTY()
	FString Location = "";

	UPROPERTY()
	bool SuccessResult = false;

	UPROPERTY()
	EFailureReasonEnum FailureReasonEnum = EFailureReasonEnum::None;

	// How much time spent to performe a specific task
	UPROPERTY()
	float TimeSpentOnTask = 0.0f;

	// Battery spent on this task
	UPROPERTY()
	float BatterySpentOnTask = 0.0f;
};

USTRUCT(BlueprintType)
struct FExperimentResult {
	GENERATED_BODY()

	UPROPERTY()
	int32 ExperimentId = 0; // preenchido por BaseExperiment

	// the experiment speed
	UPROPERTY(Transient)
	int32 ExperimentSpeed = 10;

	// will save the results
	UPROPERTY()
	float MaxWallClockInSeconds = 2000.0f;

	// Default value for the repeat experiment of
	UPROPERTY(Transient)
	int32 RepeatExperimentFor = 10;

	// TODO: REMOVE THIS. 
	// The name, can be: RoomPreparation, ...
	UPROPERTY(Transient)
	FString ExperimentName = "RoomPreparation";

	// TODO: REMOVE THIS. 
	// the scenario id, ca ben: 1, ...
	UPROPERTY(Transient)
	int32 ScenarioId = 1;

	// TODO: REMOVE THIS. 
	// the approach, ca ben: Baseline, ..
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	FString Approach = "Baseline";

	// will repeat the experiment using this number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	bool GenerateRandomProperties = false;

	// will save the results
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	bool SaveResults = false;

	// Default value for the WalClock
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	float WallClockInSeconds = 0;

	UPROPERTY()
	TArray<FTaskResult> TaskResults;

	UPROPERTY()
	TArray<FRobotProperties> Robots;

	// output json file path, provided by MutRose
	UPROPERTY(Transient)
	FString OutputJsonString = "";

	// world knowledge json file path
	UPROPERTY(Transient)
	FString WorldJsonString = "";

	// robots configuration file path
	UPROPERTY(Transient)
	FString RobotsConfigJsonString = "";

};