#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "GameFramework/Character.h"
#include "Robot.generated.h"

class ARobotController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBatteryEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskFinished);

UCLASS()
class ROBOTBT_API ARobot : public ACharacter {
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ARobotController* RobotController = nullptr;

	ARobotController* GetRobotController();

public:	
	ARobot();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString SquadName = "None";

	// the initial battery level of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float BatteryLevel = 100;

	// the initial batery discharge rate of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float BatteryDischargeRate = 10;

	// a action can consume different battery discharge rate
	UPROPERTY(EditAnywhere, Category = "Robot")
	TArray<float> ActionBatteryDischargeRate;

	// robot speed, affects the speed of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float Speed = 200;

	// process the action like cleaning a room, or organizing. This consumes battery. Each class that inherits from this class should implement this method
	UFUNCTION()
	virtual void ProcessAction();

	// indicate if the robot is moving
	UPROPERTY()
	bool IsMoving = false;

	// event that is called when the battery is over
	UPROPERTY()
	FOnBatteryEnd OnBatteryEnd;

	// event that is called when the task is finished
	UPROPERTY()
	FOnTaskFinished OnTaskFinished;

protected:
	// saves a instance of the current spline path
	UPROPERTY()
	ARoom* CurrentRoomInstace;

	// indicate if the robot finished the action of move to a specific door
	UPROPERTY()
	bool IsAtRoomLocation = false;

	// indicate if the robot move all path
	UPROPERTY()
	bool IsFinishedMovingAlongPath = false;

	UFUNCTION()
	virtual bool MoveToRoomLocation(float DeltaTime);

	UFUNCTION()
	virtual bool MoveAlongPath(float DeltaTime);

private:
	UFUNCTION()
	void ConsumeBattery(float DeltaTime);



};
