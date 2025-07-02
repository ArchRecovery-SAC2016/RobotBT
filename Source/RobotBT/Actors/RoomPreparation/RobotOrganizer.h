#pragma once

#include "CoreMinimal.h"
#include "RoomPreparation.h"
#include "../Robot.h"
#include "RobotOrganizer.generated.h"

class ARobotController;

UCLASS()
class ROBOTBT_API ARobotOrganizer : public ARobot {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ARobotOrganizer();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category="Robot")
	int32 PathIndex = 1;

	// override the GetRoomPath, because we don't want to pick the main path of the room
	virtual USplineComponent* GetRoomPath() override;

	virtual void StartNewTask(ESkillEnum SkillEnum, ARoom* Room) override;

	// Will generate randrom properties for the robot
	virtual void GenerateRandomProperties() override;

	// Go in the direction of the Furtiture
	UFUNCTION(BlueprintCallable)
	bool MoveToFurniture();

	UFUNCTION(BlueprintCallable)
	bool PlayMoveFurnitureAnimation();

	UFUNCTION(BlueprintCallable)
	bool CheckAllFurnitureMoved();

	virtual bool TaskExecution() override;

	virtual void TaskFinished() override;

	TArray<FVector> FurnitureToMoveLocation;



private:

	int32 FurnitureToMoveLocationIndex = 0;

	bool FurnitureFound = false;

	bool RobotAtFurnitureLocation = false;

	bool PlayMoveFurnitureAnimationFinished = false;

	bool AllFurnitureAtLocation = false;
	
	bool PlayMoveFurnitureAnimationIsRunning = false;

	FTimerHandle MoveFurnitureAnimationTimerHandle;

};

