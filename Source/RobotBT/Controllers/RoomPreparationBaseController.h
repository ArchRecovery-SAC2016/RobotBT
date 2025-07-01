#pragma once

#include "CoreMinimal.h"
#include "RobotBT/Actors/RoomPreparation/RobotCleaner.h"
#include "RobotBT/Actors/RoomPreparation/RobotOrganizer.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Struct/TaskStruct.h"
#include "RobotBT/Struct/ValidationStruct.h"
#include "RobotBT/Struct/WorldRoomDataStruct.h"
#include "RoomPreparationBaseController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreparationFinish, FExperimentResult, ExperimentResult);

UCLASS(minimalapi)
class ARoomPreparationBaseController : public AActor {
	GENERATED_BODY()

public:
	ARoomPreparationBaseController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	float WallClockInSeconds = 0;

	// Load tasks from file
	UFUNCTION()
	bool LoadTasksFromFile(FString NewTasksJsonString);

	// Load the world from file
	UFUNCTION()
	bool LoadWorldFromFile(FString WorldJsonString);

	UFUNCTION()
	void CurrentTaskFinished(FTaskResult TaskResult);

	// the experiment
	UFUNCTION()
	virtual void ExecuteExperiment(FExperimentResult& NewExperiment);

	UPROPERTY()
	FExperimentResult CurrentExperiment;

	FOnPreparationFinish FOnPreparationFinish;

	// saves all organizers robots instance
	UPROPERTY()
	TArray<ARobotOrganizer*> OrganizersTeam;

	// saves the robot cleaner
	UPROPERTY()
	TArray <ARobotCleaner*> CleanersRobot;

	UFUNCTION()
	TArray<ARoomPreparation*>GetRooms() { return Rooms; }

	UFUNCTION()
	FValidationStruct GetValidationStruct();

	UFUNCTION()
	ARobot* GetRobotByName(FString RobotName);

	TArray<ARobot*> GetAllRobots();

private:
	/* Saves all doors in the map	*/
	UPROPERTY()
	TArray<ARoomPreparation*> Rooms;

	/* Monitors the current room */
	UPROPERTY()
	ARoomPreparation* CurrentRoom;

	/* Get a ramndom clenaer */
	ARobotCleaner* GetRandomCleanerRobot(ESkillEnum SkillRequired);

	/* Get the best cleaner */
	ARobotCleaner* GetBestCleanerRobot(ESkillEnum SkillRequired);

	// the cleaner that was selected
	UPROPERTY()
	ARobotCleaner* CleanerRobotInst;

protected:
	// The world knowledge loaded from file
	TArray<FWorldRoomDataStruct> WorldRoomsStruct;

	// Tasks loaded from file
	UPROPERTY()
	TMap<FString, FTask> Tasks;

	int32 CurrentTaskIndex = -1;

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
	virtual void FetchRoomsToBePrepared();

	UFUNCTION()
	virtual void ExperimentFinished();

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
	virtual void PrepareWorld(FString WorldJsonString);

	// Callled when the time is over. Will notify all robots. The especialized that will implemtn this method
	virtual void TimeIsOver();

	// need this variable to calculate the wall clock
	UPROPERTY()
	float ExperimentStartTime = 0.0f;

	// robots properties used in this experiment
	UPROPERTY()
	TArray<FRobotProperties> RobotsProperties;

	bool ExperimentStarted = false;

	ARoomPreparation* GetRoomByName(FString DoorName);

	void ExecuteClean(ARobotCleaner* CleanerRobot, ARoomPreparation* RoomLocation);

	void ExecuteMoveFurniture(FString RobotName, ARoomPreparation* RoomLocation);

	void ExecuteOpenDoor(ARobotCleaner* CleanerRobot, ARoomPreparation* RoomLocation);

	void ExecuteSanitizeRobot(ARobotCleaner* CleanerRobot, ARoomPreparation* RoomLocation);

	bool EvaluatePreCondition(const FPredicate& Predicate);
};





