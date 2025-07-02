
#include "RobotController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Navigation/PathFollowingComponent.h"
#include "RobotBT/Actors/Robot.h"

ARobotController::ARobotController() {
	
}

void ARobotController::BeginPlay() {
	Super::BeginPlay();

	ControlledPawn = Cast<ARobot>(GetPawn());

	UBlackboardComponent* MyBlackboard = GetBlackboardComponent();
	if (!MyBlackboard) {
		MyBlackboard->SetValueAsObject(TEXT("SelfActor"), ControlledPawn);
	}

    CurrentDistanceAlongSpline = 0.0f;
}

void ARobotController::OnPossess(APawn* InPawn) {
    Super::OnPossess(InPawn);

	// set the behaviour tree for the robot
    ARobot* Robot = Cast<ARobot>(InPawn);
    if (Robot && Robot->BehaviorTreeAsset) {
        RunBehaviorTree(Robot->BehaviorTreeAsset);
    }
}

bool ARobotController::MoveToActorLocation(AActor* MoveToLocation) {
	if (ControlledPawn == nullptr) return false;

	float Distance = (ControlledPawn->GetActorLocation() - MoveToLocation->GetActorLocation()).Size();

	if (Distance < 50) {
		return true;
	}

	MoveToActor(MoveToLocation, 50, true, true, false, DefaultNavigationFilterClass, true);

	return false;
}

bool ARobotController::MoveToNewLocation(FVector NewLocation) {
    if (ControlledPawn == nullptr) return false;

    // Configurar um resultado para o movimento
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(NewLocation);
    MoveRequest.SetAcceptanceRadius(10.0f); // Tolerância para considerar que chegou

    FNavPathSharedPtr NavPath;
    EPathFollowingRequestResult::Type MoveResult = MoveTo(MoveRequest, &NavPath);

    if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal) {
        return true;
    }

    return false;
}

void ARobotController::RotateToFaceActor(const AActor* ActorSelected) {
	if (ControlledPawn == nullptr) return;

	// e direction the enemy should move in
	const FVector PlayerDirection = ActorSelected->GetActorLocation() - ControlledPawn->GetActorLocation();
	const FRotator NewRotation = PlayerDirection.Rotation();
	//  Refresh the rotation to face the player
	ControlledPawn->SetActorRotation(FRotator(NewRotation.Pitch, NewRotation.Yaw, 0.0f));
}

bool ARobotController::MoveAlongSpline(USplineComponent* Spline) {
    if (!Spline || !ControlledPawn) return false;

    const int32 NumPoints = Spline->GetNumberOfSplinePoints();

    // Se já passou do último ponto
    if (CurrentPathIndex >= NumPoints) {
        CurrentPathIndex = 0;
        return true;
    }

    // Localização do ponto atual
    FVector TargetLocation = Spline->GetLocationAtSplinePoint(CurrentPathIndex, ESplineCoordinateSpace::World);

    // Tenta se mover
    bool bArrived = MoveToNewLocation(TargetLocation) ;

    if (bArrived) {
        CurrentPathIndex++; // Vai pro próximo ponto
    }

    return false; // Ainda está se movendo
}

void ARobotController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
    Super::OnMoveCompleted(RequestID, Result);

    // Se o movimento foi concluído com sucesso, incrementa o índice
    if (Result.Code == EPathFollowingResult::Success) {
        CurrentPathIndex++;
    }
}

