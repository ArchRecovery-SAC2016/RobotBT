#pragma once

#include "CoreMinimal.h"
#include "DoorSensor.h"
#include "GameFramework/Character.h"
#include "Robot.generated.h"

class ARobotController;

UCLASS()
class ROBOTBT_API ARobot : public ACharacter {
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ADoorSensor* RoomSelected;

	void GoFrontOfRoom(const FVector& RoomLocation);

	void GoCenterOfRoom(const FVector& RoomLocation);

	void GoOutsideOfRoom(const FVector& RoomLocation);

	// Controls if Go to the front of the finished
	bool bFrontOfRoom = false;

	// Controls if Go to the center of the room finished
	bool bCenterOfRoom = false;

	// Controls if Go to outside Room finished
	bool bOutsideRoom = false;

	UPROPERTY()
	ARobotController* RobotController;

	ARobotController* GetRobotController();

public:	
	ARobot();

	virtual void Tick(float DeltaTime) override;

	/* The child will know how to process the action */
	UFUNCTION()
	virtual bool ProcessAction();


	
	

};
