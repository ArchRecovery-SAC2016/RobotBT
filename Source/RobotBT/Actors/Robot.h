#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "Components/WidgetComponent.h"
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

	// the widget that will be used to show the robot information
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class URobotWidget> RobotWidgetWBP;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString SquadName = "None";

	// the initial battery level of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float BatteryLevel = 1;

	// the initial batery discharge rate of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float BatteryDischargeRate = 0.01;

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

	UFUNCTION()
	virtual ARoom* GetRoom();

	UFUNCTION()
	virtual void SetRoom(ARoom* NewRoomInstance) { RoomInstace = NewRoomInstance; }

	UFUNCTION()
	virtual USplineComponent* GetRoomPath();

	UFUNCTION()
	void UpdateRobotWidget();
	URobotWidget* GetRobotWidget();

	UPROPERTY(EditAnywhere)
	bool ShowWidget = true;

protected:
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

	UFUNCTION()
	void UpdateCurrentActionText(FString NewAction);

private:
	UFUNCTION()
	void ConsumeBattery(float DeltaTime);

	// saves a instance of the current spline path
	UPROPERTY()
	ARoom* RoomInstace;

	// saves a instance of the Robot Widget
	UPROPERTY()
	URobotWidget* RobotWidget = nullptr;

	UPROPERTY()
	FText CurrentAction;



};
