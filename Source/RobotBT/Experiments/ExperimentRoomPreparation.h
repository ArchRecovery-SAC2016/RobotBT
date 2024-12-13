#pragma once

#include "CoreMinimal.h"
#include "Experiment.h"
#include "RobotBT/Actors/RoomPreparation/RobotCleaner.h"
#include "RobotBT/Actors/RoomPreparation/RobotOrganizer.h"
#include "ExperimentRoomPreparation.generated.h"

UCLASS(minimalapi)
class AExperimentRoomPreparation : public AExperiment
{
	GENERATED_BODY()

public:
	AExperimentRoomPreparation();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// saves all organizers robots instance
	UPROPERTY()
	TArray<ARobotOrganizer*> OrganizersTeam;

	// saves the robot cleaner
	UPROPERTY()
	ARobotCleaner* CleanerRobot;

	UFUNCTION()
	TArray<ARoomPreparation*>GetRooms() { return Rooms; }

private:
	/* Saves all doors in the map	*/
	UPROPERTY()
	TArray<ARoomPreparation*> Rooms;

	virtual void FetchRoomsToBePrepared() override;

	virtual void ExecuteCurrentDecomposition() override;

	ARoomPreparation* GetRoomByName(FString DoorName);

	void ExecuteClean(FString RobotName, ARoomPreparation* RoomLocation);

	void ExecuteMoveFurniture(FString RobotName, ARoomPreparation* RoomLocation);

	void ExecuteOpenDoor(FString RobotName, ARoomPreparation* RoomLocation);

	void ExecuteSanitizeRobot(FString RobotName, ARoomPreparation* RoomLocation);

	virtual bool CheckPreCondition(FTask* NewTask) override;

	bool EvaluatePreCondition(const FPredicate& Predicate);

	virtual void PrepareWorld() override;

	virtual void TimeIsOver() override;

};