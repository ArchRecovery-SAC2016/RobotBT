#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "RobotBT/Actors/Robot.h"
#include "Components/SplineComponent.h"
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
	bool MoveToNewLocation(const FVector& Vector, float DeltaTime);

	UFUNCTION()
	void RotateToFaceActor(const AActor* ActorSelected);

	UFUNCTION()
	void ProcessAction();

	UFUNCTION()
	bool MoveAlongSpline(USplineComponent* SplineComponent, int32 StartIndex, int32 EndIndex, float DeltaTime);

	UPROPERTY()
	ARobot* ControlledPawn;

private:
	bool ActionFinished = false;

	float DistanceAlongSpline; // Declare a vari�vel de membro


};


