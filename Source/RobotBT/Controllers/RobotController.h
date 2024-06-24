#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "RobotBT/Actors/Robot.h"
#include "RobotController.generated.h"

class ADoorSensor;

UCLASS()
class ARobotController : public AAIController {
	GENERATED_BODY()

public:
	ARobotController();

	virtual void BeginPlay() override;

	bool MoveToActorLocation(AActor* MoveToLocation);

	UFUNCTION()
	void RotateToFaceActor(const AActor* ActorSelected);

	bool ProcessAction();

	UPROPERTY()
	ARobot* ControlledPawn;

private:
	bool ActionFinished = false;


};


