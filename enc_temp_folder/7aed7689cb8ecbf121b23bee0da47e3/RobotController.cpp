
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
}

bool ARobotController::MoveToActorLocation(AActor* MoveToLocation) {
	if (ControlledPawn == nullptr) return false;

	float Distance = (ControlledPawn->GetActorLocation() - MoveToLocation->GetActorLocation()).Size();

	if (Distance < 10) {
		return true;
	}

	MoveToActor(MoveToLocation, 10, true, true, false, DefaultNavigationFilterClass, true);

	return false;
}

bool ARobotController::MoveToNewLocation(FVector NewLocation) {
    if (ControlledPawn == nullptr) return false;

    // Configurar um resultado para o movimento
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(NewLocation);
    MoveRequest.SetAcceptanceRadius(5.0f); // Tolerância para considerar que chegou

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



bool ARobotController::MoveAlongSpline(USplineComponent* Spline, float Speed) {
    if (!Spline || !ControlledPawn) {
        UE_LOG(LogTemp, Warning, TEXT("SplineComponent or ControlledPawn is null!"));
        return false;
    }

    float SplineLength = Spline->GetSplineLength();
    float DeltaTime = GetWorld()->GetDeltaSeconds();

    if (CurrentDistanceAlongSpline >= SplineLength) {
		CurrentDistanceAlongSpline = 0.0f; // Reinicia a distância se já chegou ao fim
        return true; // Chegou ao fim da spline
    }

    // Avança distância suavemente
    CurrentDistanceAlongSpline = FMath::Min(CurrentDistanceAlongSpline + Speed * DeltaTime, SplineLength);

    FTransform SplineTransform = Spline->GetTransformAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);

    // Movimento e rotação suave
    FVector NewLocation = SplineTransform.GetLocation();
    FRotator TargetRotation = SplineTransform.GetRotation().Rotator();
    FRotator CurrentRotation = ControlledPawn->GetActorRotation();
    FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 3.0f);

    ControlledPawn->SetActorLocation(NewLocation);
    ControlledPawn->SetActorRotation(SmoothedRotation);

    return false; // Ainda está se movendo
}



void ARobotController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
    Super::OnMoveCompleted(RequestID, Result);

    // Se o movimento foi concluído com sucesso, incrementa o índice
    if (Result.Code == EPathFollowingResult::Success) {
        CurrentPathIndex++;
    }
}

