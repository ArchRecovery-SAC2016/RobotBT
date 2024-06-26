#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Struct/RoomPreparationStruct.h"
#include "Actors/DoorSensor.h"
#include "Actors/RobotCleaner.h"
#include "Actors/RobotOrganizer.h"
#include "Enum/ActionsEnum.h"
#include "Enum/MessageColorEnum.h"
#include "Struct/TaskStruct.h"
#include "Widget/WorldKnowledgeWidget.h"
#include "RobotBTGameMode.generated.h"


UCLASS(minimalapi)
class ARobotBTGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARobotBTGameMode();


	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// saves all organizers robots instance
	TArray<ARobotOrganizer*> OrganizersTeam;

	// saves the robot cleaner
	ARobotCleaner* CleanerRobot;

	UFUNCTION()
	void UpdateWorldKnowledgeWidget();

	UFUNCTION()
	TArray<ADoorSensor*>GetDoors() { return DoorSensors; }

	UFUNCTION()
	ADoorSensor* GetTaskRoom();

	// Load tasks from file
	UFUNCTION()
	void LoadTasksFromFile();

private:

	/* Saves all doors in the map	*/
	UPROPERTY()
	TArray<ADoorSensor*> DoorSensors;

	UFUNCTION()
	UWorldKnowledgeWidget * GetWorldKnowledgeWidget();

	UPROPERTY()
	UWorldKnowledgeWidget* WorldKnowledgeWidgetInst;

	UPROPERTY()
	TMap<FString, FTask> Tasks;

	int32 CurrentTaskIndex;

	FTask* CurrentTask;

	// Get the next task to be executed and check if it is possible to execute it
	FTask* GetNextTask();

	void StartFunitureTask(ADoorSensor* Room);

	// After the task is choosed, this method will execute it in the tick function
	void ExecuteCurrentTask();

	// saves the current task decomposition
	TArray<FTaskDecomposition> DecompositionQueue;

	// saves the current index, so we can control the decomposition
	int32 CurrentDecompositionIndex;

	// All task have preconditions. This method check if the preconditions are satisfied
	bool CheckPreCondition(FTask* NewTask);

	bool ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition);

	ADoorSensor* GetDoorByName(const FString& DoorName);

	bool ExperimentIsOver = false;

	// is a bind function. Called when a task of the robot is finished
	UFUNCTION()
	void OnRoomCleaned(bool bNewState);

	// is a bind function. Called when a task of the robot is finished
	UFUNCTION()
	void OnDoorOpened(bool bNewState);

	// is a bind function. Called when a task of the robot sanitized is finished
	UFUNCTION()
	void OnRobotSanitized(bool bNewState);

	
};





