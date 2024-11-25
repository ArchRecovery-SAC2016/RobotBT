#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Struct/ExperimentResult.h"
#include "Struct/TaskStruct.h"
#include "Struct/WorldRoomDataStruct.h"
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
	virtual void LoadTasksFromFile(FString Experiment, int32 ScenarioId);

	// Load the world from file
	UFUNCTION()
	bool LoadWorldFromFile(FString Experiment, int32 ScenarioId);

	UFUNCTION()
	void CurrentTaskFinished(FTaskResult TaskResult);

	FExperimentResult Experiment;

	virtual FTask* GetNextTask();
protected:
	// The world knowledge loaded from file
	TArray<FWorldRoomDataStruct> WorldRoomsStruct;

	UPROPERTY()
	TMap<FString, FTask> Tasks;

	int32 CurrentTaskIndex= - 1;

	// Some task need 2 robots. So for the sucess, the 2 robots need to return true. This counter controls this 
	int32 NumberOfTask = 1;

	FTask* CurrentTask;

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

	// when the experiment is over, this variable will be setted
	bool ExperimentIsOver = false;

	// used to indicate the Experiment Id. -1 will be a single experiment
	int32 ExperimentId  = -1;

	// will repet the experiment using this number
	int32 RepeatExperiment = 1000;

	// will repet the experiment using this number
	bool GenerateRandomProperties = false;

	UFUNCTION()
	virtual void StartExperiment(int32 numberOfTimes) {};

	UFUNCTION()
	virtual void ExecuteNextExperiment() {};

	// Called when everything is finished
	UFUNCTION()
	virtual void FinishExperiment();


};





