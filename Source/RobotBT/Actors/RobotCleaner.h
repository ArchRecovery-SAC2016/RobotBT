#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "RobotBT/Controllers/RobotController.h"
#include "RobotCleaner.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomCleaned, bool, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpened, bool, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSanitized, bool, NewState);

UCLASS()
class ROBOTBT_API ARobotCleaner : public ARobot {
	GENERATED_BODY()


public:

	ARobotCleaner();

	void BeginPlay();

	
	void Tick(float DeltaSeconds) override;

	UPROPERTY()
	bool IsCleaning = false;

	bool IsOpeningDoor = false;

	bool IsSanitizing = false;

	UFUNCTION()
	void DoorOpenCompleted(bool bNewState);

	// Called by the Game Mode to start the cleaning room action. Will set the 
	void StartOpeningDoor(ADoorSensor* RoomSelected);

	UFUNCTION()
	void StartCleaningRoom(ADoorSensor* RoomSelected);

	UFUNCTION()
	void StartSanitize(ADoorSensor* RoomSelected);

	FOnRoomCleaned OnRoomCleaned;

	FOnDoorOpened OnDoorOpened;

	FOnSanitized OnRobotSanitized;

private:

	ARobotController* RobotController;

	UPROPERTY()
	ADoorSensor* RoomSelected;

	// Called by the Event tick. Will clean the room selected. Return true if the room is clean and false if it is not
	void CleanRoom();

	// Called by the Event tick. Will open the room selected. Return true if the room is open and false if it is not
	void OpenRoom();

	// Called by Event tick. Will go to the room selected and sanitize the robot
	void SanitizeRoom();



};
