#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "RobotBT/Controllers/RobotController.h"
#include "RobotCleaner.generated.h"

UCLASS()
class ROBOTBT_API ARobotCleaner : public ARobot {
	GENERATED_BODY()


public:

	ARobotCleaner();
	void BeginPlay();

	UPROPERTY()
	bool IsCleaning = false;
	
	// return true when the action is finished and false when it is not
	virtual bool ProcessAction() override;

	// Called by the Event tick from the Game Mode. Will clean the room selected. Return true if the room is clean and false if it is not
	bool CleanRoom(ADoorSensor* RoomSelected);


private:

	ARobotController* RobotController;




};
