#pragma once

#include "Actors/Room.h"
#include "CoreMinimal.h"
#include "RobotBTGameMode.h"
#include "Actors/RoomPreparation/RobotCleaner.h"
#include "Actors/RoomPreparation/RobotOrganizer.h"
#include "Struct/MovePlanStruct.h"
#include "RoomPreparationExperiment.generated.h"

UCLASS(minimalapi)
class ARoomPreparationExperiment : public ARobotBTGameMode
{
	GENERATED_BODY()

public:
	ARoomPreparationExperiment();

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

	virtual void ExecuteCurrentDecomposition() override;

	ARoomPreparation* GetRoomByName(FString DoorName);

	bool ExperimentIsOver = false;

	void ExecuteClean(FString RobotName, ARoomPreparation* RoomLocation);

	void ExecuteMoveFurniture(FString RobotName, ARoomPreparation* RoomLocation);

	void ExecuteOpenDoor(FString RobotName, ARoomPreparation* RoomLocation);

	void ExecuteSanitizeRobot(FString RobotName, ARoomPreparation* RoomLocation);

};





