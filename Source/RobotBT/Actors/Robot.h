#pragma once

#include "CoreMinimal.h"
#include "SplinePath.h"
#include "GameFramework/Character.h"
#include "Robot.generated.h"

class ARobotController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatteryOver, bool, NewState);

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
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString SquadName = "None";

	// the initial battery level of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float BatteryLevel = 100;

	// the initial batery discharge rate of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float BatteryDischargeRate = 10;

	UPROPERTY(EditAnywhere, Category = "Robot")
	TArray<float> ActionBatteryDischargeRate;

	// robot speed, affects the speed of the robot
	UPROPERTY(EditAnywhere, Category = "Robot")
	float Speed = 200;

	// process the action like cleaning a room, or organizing. This consumes battery. Each class that inherits from this class should implement this method
	UFUNCTION()
	virtual void ProcessAction();

	// set the new spline path
	UFUNCTION()
	void SetSplinePath(ASplinePath* NewSplinePath) { CurrentSplinePath = NewSplinePath; }

	// indicate if the robot is moving
	UPROPERTY()
	bool IsMoving = false;

	// event that is called when the battery is over
	FOnBatteryOver OnBatteryOver;

private:
	// saves a instance of the current spline path
	UPROPERTY()
	ASplinePath* CurrentSplinePath = nullptr;

	UFUNCTION()
	void ConsumeBattery(float DeltaTime);


};
