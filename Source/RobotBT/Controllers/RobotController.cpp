
#include "RobotController.h"

#include "Components/SplineComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "RobotBT/Actors/Robot.h"


ARobotController::ARobotController() {
	
}

void ARobotController::BeginPlay() {
	Super::BeginPlay();

	ControlledPawn = Cast<ARobot>(GetPawn());
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

void ARobotController::RotateToFaceActor(const AActor* ActorSelected) {
	if (ControlledPawn == nullptr) return;

	// e direction the enemy should move in
	const FVector PlayerDirection = ActorSelected->GetActorLocation() - ControlledPawn->GetActorLocation();
	const FRotator NewRotation = PlayerDirection.Rotation();
	//  Refresh the rotation to face the player
	ControlledPawn->SetActorRotation(FRotator(NewRotation.Pitch, NewRotation.Yaw, 0.0f));
}

void ARobotController::ProcessAction() {
	// do something to change the action Finish event, like some animation
	if (ControlledPawn == nullptr) return;

	return ControlledPawn->ProcessAction();
	
}

bool ARobotController::MoveToNewLocation(const FVector& NewPositionVector) {
	if (ControlledPawn == nullptr) return false;

	float Distance = (ControlledPawn->GetActorLocation() - NewPositionVector).Size();

	if (Distance < 100) {
		return true;
	}

	MoveToLocation(NewPositionVector, 1, true, true);

	return false;
}

bool ARobotController::MoveAlongSpline(USplineComponent* SplineComponent, int32 StartIndex, int32 EndIndex, float DeltaTime) {
	if (SplineComponent == nullptr) {
		return false; // Verifica��o de seguran�a caso o spline seja inv�lido
	}

	// Calcula as dist�ncias de in�cio e fim ao longo do spline
	float StartDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(StartIndex);
	float EndDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(EndIndex);

	// Garante que a dist�ncia inicial seja menor que a final
	if (EndDistance < StartDistance) {
		Swap(StartDistance, EndDistance);
	}

	// Verifica se chegou ao final
	if (DistanceAlongSpline >= EndDistance) {
		DistanceAlongSpline = EndDistance;
		return true; // Retorna true ao atingir o ponto final
	}

	// Atualiza a dist�ncia ao longo do spline
	DistanceAlongSpline += ControlledPawn->Speed * DeltaTime; // Incrementa a dist�ncia
	if (DistanceAlongSpline > EndDistance) {
		DistanceAlongSpline = EndDistance; // Limita a dist�ncia
	}

	// Atualiza a posi��o e rota��o do ator ao longo do spline
	FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	FRotator NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	// Move o pr�prio ator
	ControlledPawn->SetActorLocation(NewLocation);
	ControlledPawn->SetActorRotation(NewRotation);

	return false; // N�o chegou ao final ainda
}