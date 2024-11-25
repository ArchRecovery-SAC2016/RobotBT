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


	/** Start Experiment Properties. */
	/** Must be set on the GM blueprint. Example: GM_ExperimentRoomPreparation */

		// will repeat the experiment for this number 
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

	/** End Experiment Properties. */

	// Load tasks from file
	UFUNCTION()
	virtual void LoadTasksFromFile();

	// Load the world from file
	UFUNCTION()
	bool LoadWorldFromFile();

	UFUNCTION()
	void CurrentTaskFinished(FTaskResult TaskResult);

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

	// used to indicate the Experiment Id. -1 will be a single experiment
	int32 ExperimentId  = -1;

	// saves the result of the experiment
	FExperimentResult Experiment;

	// try to execute the next task. If fails, the experiment is over
	virtual FTask* GetNextTask();

	// set the initial data for the experiment
	UFUNCTION()
	virtual void StartExperiment() {};

	// the experiment
	UFUNCTION()
	virtual void ExecuteNextExperiment();

	// Called when everything is finished
	UFUNCTION()
	virtual void FinishExperiment();

	// After the task is choosed, this method will execute it, creating the decomposition
	virtual void ExecuteCurrentTask();

	// will execute the decomposition of the task
	virtual void ExecuteCurrentDecomposition();

	// All task have preconditions. This method check if the preconditions are satisfied
	virtual bool CheckPreCondition(FTask* NewTask);

	virtual bool ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition);

	// the especialized class will implement this method
	virtual void SetRandomRobotsProperties() {};

	// the especialized class will implement this method
	virtual void PrepareWorld() {};
};





