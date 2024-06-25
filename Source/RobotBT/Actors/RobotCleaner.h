#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "RobotBT/Controllers/RobotController.h"
#include "RobotCleaner.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomCleaned, bool, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpened, bool, NewState);

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
	
	// return true when the action is finished and false when it is not
	virtual bool ProcessAction() override;

	void StartCleaningRoom(ADoorSensor* RoomSelected);

	void StarOpeningDoor(ADoorSensor* RoomSelected);

	FOnRoomCleaned OnRoomCleaned;

	FOnDoorOpened OnDoorOpened;

private:

	ARobotController* RobotController;

	UPROPERTY()
	ADoorSensor* RoomSelected;

	// Called by the Event tick from the Game Mode. Will clean the room selected. Return true if the room is clean and false if it is not
	bool CleanRoom();

	// Called by the Event tick from the Game Mode. Will open the room selected. Return true if the room is open and false if it is not
	bool OpenRoom();



};
