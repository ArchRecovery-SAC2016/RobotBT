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

	UFUNCTION()
	bool MoveToActorLocation(AActor* MoveToLocation);

	UFUNCTION()
	bool MoveToNewLocation(FVector NewLocation);

	UFUNCTION()
	void RotateToFaceActor(const AActor* ActorSelected);

	UFUNCTION()
	bool MoveAlongSpline(USplineComponent* SplineComponent, int32 StartIndex, int32 EndIndex);

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
	ARobot* ControlledPawn;

private:
	bool ActionFinished = false;

	float DistanceAlongSpline = 0; // Declare a variável de membro

	float CurrentDistance = 0.0f;  // Armazena a distância atual percorrida

	int32 CurrentPathIndex = -1;  // Armazena o índice atual na spline
};


