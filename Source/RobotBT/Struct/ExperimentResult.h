#pragma once

#include "CoreMinimal.h"
#include "RobotProperties.h"
#include "ValidationStruct.h"
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

UENUM(BlueprintType)
enum class EnumResultFinal : uint8 {
	NotProcessed        UMETA(DisplayName = "Not Processed"),         // Nunca chegou a processar o resultado
	
	ValidationResult               UMETA(DisplayName = "Validation Result"),          // Chamou mas validação retornou um resultado
	ValidationCallFailed           UMETA(DisplayName = "Validation Call Failed"),     // Não conseguiu nem chamar a validação

	CausalAnalysisResult			UMETA(DisplayName = "Causal Analysis Result"),  // Chamou o causal analysis e deu esse resultado
	CausalAnalysisCallFailed       UMETA(DisplayName = "Causal Analysis Call Failed") // Tentou chamar mas deu erro
};

USTRUCT(BlueprintType)
struct FResultFinal {
	GENERATED_BODY()

	// mark if the experiment was successful or not
	UPROPERTY(BlueprintReadOnly)
	EnumResultFinal ResultEnum = EnumResultFinal::NotProcessed;

	// the general description of the result. 
	UPROPERTY(BlueprintReadOnly)
	FString Description = "";
};

USTRUCT(BlueprintType)
struct FExperimentResult {
	GENERATED_BODY()

	// TODOl: DELETAR. TAH USANDO O ExperimentInstance->CurrentExperiment.ExperimentId
	UPROPERTY(BlueprintReadOnly)
	int32 ExperimentId = 0; // preenchido por BaseExperiment

	// the experiment speed
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 ExperimentSpeed = 2;

	// will save the results
	UPROPERTY(BlueprintReadOnly)
	float MaxWallClockInSeconds = 400.0f;

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
	UPROPERTY()
	FString OutputTasksJsonString = "";

	// world knowledge json file path
	UPROPERTY()
	FString WorldJsonString = "";
	
	// robots configuration file path
	UPROPERTY(Transient)
	FString RobotsConfigJsonString = "";

	// the final result. Contains information about the experiment, like success or failure
	UPROPERTY(BlueprintReadOnly)
	FResultFinal ResultFinal = FResultFinal();


	// Whitch data was sent by the validation
	UPROPERTY(BlueprintReadOnly, Category = "Experiment")
	FValidationStruct ValidationStruct;
};