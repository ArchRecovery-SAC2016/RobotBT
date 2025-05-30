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
struct FResultFinal {
	GENERATED_BODY()

	// mark if the experiment was successful or not
	UPROPERTY(BlueprintReadOnly)
	bool Success = false;

	// mark if managed to validate the experiment, calling the API
	UPROPERTY(BlueprintReadOnly)
	bool ManagedToValidate = false;

	// if the result failed, this will contain the reason
	UPROPERTY(BlueprintReadOnly)
	EFailureReasonEnum FailureReasonEnum = EFailureReasonEnum::None;

	// the root cause of the failure, if any
	UPROPERTY(BlueprintReadOnly)
	FString RootCauseAnalysis = "None";

	// the general description of the result. 
	UPROPERTY(BlueprintReadOnly)
	FString ValidationResult = "None";
};

USTRUCT(BlueprintType)
struct FExperimentResult {
	GENERATED_BODY()

	// TODOl: DELETAR. TAH USANDO O ExperimentInstance->CurrentExperiment.ExperimentId
	UPROPERTY(BlueprintReadOnly)
	int32 ExperimentId = 0; // preenchido por BaseExperiment

	// the experiment speed
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 ExperimentSpeed = 10;

	// will save the results
	UPROPERTY(BlueprintReadOnly)
	float MaxWallClockInSeconds = 200.0f;

	// Default value for the repeat experiment of
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 RepeatExperimentFor = 10;

	// TODO: REMOVE THIS. 
	// The name, can be: RoomPreparation, ...
	UPROPERTY(Transient, BlueprintReadOnly)
	FString ExperimentName = "RoomPreparation";

	// TODO: REMOVE THIS. 
	// the scenario id, ca ben: 1, ...
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 ScenarioId = 1;

	// TODO: REMOVE THIS. 
	// the approach, ca ben: Baseline, ..
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyExperiment")
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

	UPROPERTY(BlueprintReadOnly)
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

	// the final result. Contains information about the experiment, like success or failure
	UPROPERTY(Transient, BlueprintReadOnly)
	FResultFinal ResultFinal = FResultFinal();
};