#pragma once

#include "CoreMinimal.h"
#include "SplinePath.h"
#include "GameFramework/Character.h"
#include "RobotBT/Struct/MovePlanStruct.h"
#include "Robot.generated.h"

class ARobotController;

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

	UPROPERTY()
	int32 CurrentMovePlanIndex = 0;

	// The move plan that the robot will execute
	UPROPERTY(EditInstanceOnly)
	TArray<FMovePlanStruct> MovePlan;

	// the initial battery level of the robot
	UPROPERTY(EditInstanceOnly)
	float BatteryLevel = 100;

	// the initial batery discharge rate of the robot
	UPROPERTY(EditInstanceOnly)
	float BatteryDischargeRate = 10;

	// robot speed, affects the speed of the robot
	UPROPERTY(EditInstanceOnly)
	float Speed = 200;

	// saves the spline path responsible to the robot movement
	UPROPERTY(EditInstanceOnly)
	TArray<ASplinePath*> SplinePath;

	// get the spline path by the identifier
	UFUNCTION()
	ASplinePath* GetPathByIndentifier(FString Identifier);

	// start executing a new 
	UFUNCTION()
	bool ExecuteNextMovePlan();

	// called when the current move plan finished
	UFUNCTION()
	void OnCurrentMovePlanFinished();

	// process the action like cleaning a room, or organizing. This consumes battery. Each class that inherits from this class should implement this method
	UFUNCTION()
	virtual void ProcessAction();

private:
	// saves a instance of the current spline path
	UPROPERTY()
	ASplinePath* CurrentSplinePath = nullptr;

};
