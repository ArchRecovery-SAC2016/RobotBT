#pragma once

#include "CoreMinimal.h"
#include "Enum/FailureReasonEnum.h"
#include "GameFramework/GameModeBase.h"
#include "Struct/RobotProperties.h"
#include "Struct/TaskStruct.h"
#include "BaseExperiment.generated.h"

UCLASS(minimalapi)
class ABaseExperiment : public AGameModeBase {
	GENERATED_BODY()

public:
	ABaseExperiment();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Load tasks from file
	UFUNCTION()
	virtual void LoadTasksFromFile();

	UFUNCTION()
	void CurrentTaskFinished(FRobotProperties RobotProperties);

	UFUNCTION()
	void CurrentTaskFailed(EFailureReasonEnum FailureReason, FRobotProperties RobotProperties);

protected:
	UPROPERTY()
	TMap<FString, FTask> Tasks;

	int32 CurrentTaskIndex;

	FTask* CurrentTask;

	// Get the next task to be executed and check if it is possible to execute it
	virtual FTask* GetNextTask();

	// After the task is choosed, this method will execute it, creating the decomposition
	virtual void ExecuteCurrentTask();

	// will execute the decomposition of the task
	virtual void ExecuteCurrentDecomposition();

	// saves the current task decomposition
	TArray<FTaskDecomposition> DecompositionQueue;

	// saves the current index, so we can control the decomposition
	int32 CurrentDecompositionIndex;

	// All task have preconditions. This method check if the preconditions are satisfied
	virtual bool CheckPreCondition(FTask* NewTask);

	virtual bool ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition);

	bool ExperimentIsOver = false;
};





