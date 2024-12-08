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
	int32 ExperimentId = -1; // preenchido por BaseExperiment

	UPROPERTY()
	float ExperementSpeed = 100; // The Speed of the experiment

	UPROPERTY()
	float WallClockInSeconds = 0; // The experiment timer

	/** Start Experiment Properties. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	int32 RepeatExperimentFor = 1;

	// The name, can be: RoomPreparation, ...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	FString ExperimentName = "RoomPreparation";

	// the scenario id, ca ben: 1, ...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	int32 ScenarioId = 1;

	// the approach, ca ben: Baseline, ..
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	FString Approach = "Baseline";

	// the approach, ca ben: Baseline, ..
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	int32 ExperimentSpeed = 1;

	// will repeat the experiment using this number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	bool GenerateRandomProperties = false;

	// will save the results
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	bool SaveResults = false;

	// will save the results
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyExperiment")
	float MaxWallClockInSeconds = 2000.0f;

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