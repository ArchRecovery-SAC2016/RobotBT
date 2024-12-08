#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Struct/TaskStruct.h"
#include "RobotBT/Struct/WorldRoomDataStruct.h"
#include "Experiment.generated.h"

UCLASS(minimalapi)
class AExperiment : public AGameModeBase {
	GENERATED_BODY()

public:
	AExperiment();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Load tasks from file
	UFUNCTION()
	virtual void LoadTasksFromFile();

	// Load the world from file
	UFUNCTION()
	bool LoadWorldFromFile();

	UFUNCTION()
	void CurrentTaskFinished(FTaskResult TaskResult);

	// the experiment
	UFUNCTION()
	virtual void ExecuteExperiment(FExperimentResult NewExperiment);

protected:
	// The world knowledge loaded from file
	TArray<FWorldRoomDataStruct> WorldRoomsStruct;

	UPROPERTY()
	TMap<FString, FTask> Tasks;

	int32 CurrentTaskIndex= - 1;

	// Some task need 2 robots. So for the sucess, the 2 robots need to return true. This counter controls this 
	int32 NumberOfTask = 1;

	FTask* CurrentTask;

	// saves the current task decomposition
	TArray<FTaskDecomposition> DecompositionQueue;

	// saves the current index, so we can control the decomposition
	int32 CurrentDecompositionIndex;

	// when the experiment is over, this variable will be setted
	bool ExperimentIsOver = false;

	// try to execute the next task. If fails, the experiment is over
	virtual FTask* GetNextTask();

	UFUNCTION()
	virtual void ExperimentFinished();

	// After the task is choosed, this method will execute it, creating the decomposition
	virtual void ExecuteCurrentTask();

	// will execute the decomposition of the task
	virtual void ExecuteCurrentDecomposition() { };

	// All task have preconditions. This method check if the preconditions are satisfied
	virtual bool CheckPreCondition(FTask* NewTask) { return true; }

	virtual bool ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition);

	// the especialized class will implement this method
	virtual void SetRandomRobotsProperties() {};

	// the especialized class will implement this method
	virtual void PrepareWorld() {};

	// Callled when the time is over. Will notify all robots. The especialized that will implemtn this method
	virtual void TimeIsOver() {};

	// need this variable to calculate the wall clock
	UPROPERTY()
	float ExperimentStartTime = 0.0f;

	// robots properties used in this experiment
	UPROPERTY()
	TArray<FRobotProperties> RobotsProperties;

	FExperimentResult CurrentExperiment;

	bool ExperimentStarted = false;
};





