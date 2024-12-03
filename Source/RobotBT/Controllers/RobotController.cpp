
#include "RobotController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
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

void ARobotController::RotateToFaceActor(const AActor* ActorSelected) {
	if (ControlledPawn == nullptr) return;

	// e direction the enemy should move in
	const FVector PlayerDirection = ActorSelected->GetActorLocation() - ControlledPawn->GetActorLocation();
	const FRotator NewRotation = PlayerDirection.Rotation();
	//  Refresh the rotation to face the player
	ControlledPawn->SetActorRotation(FRotator(NewRotation.Pitch, NewRotation.Yaw, 0.0f));
}

bool ARobotController::MoveAlongSpline(USplineComponent* SplineComponent, int32 StartIndex, int32 EndIndex) {
	if (SplineComponent == nullptr || ControlledPawn == nullptr) {
		return false;
	}

	// Inicializa as vari�veis somente na primeira chamada
	if (CurrentPathIndex == -1) {
		CurrentPathIndex = StartIndex;
		CurrentDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(CurrentPathIndex);
	}

	// Obt�m a posi��o atual do ator para preservar a coordenada Z
	FVector CurrentLocation = ControlledPawn->GetActorLocation();

	// Verifica se j� chegou ao final (ignorando Z)
	float EndDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(EndIndex);
	FVector EndLocation = SplineComponent->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::World);
	EndLocation.Z = CurrentLocation.Z; // Ignora Z na valida��o

	FVector CurrentTargetLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	CurrentTargetLocation.Z = CurrentLocation.Z; // Ignora Z na valida��o

	if (FVector::Dist2D(CurrentLocation, EndLocation) < KINDA_SMALL_NUMBER) {
		CurrentPathIndex = -1;  // Reset para a pr�xima chamada
		CurrentDistance = 0.0f; // Garante que a dist�ncia tamb�m seja resetada
		return true;            // Movimento conclu�do
	}

	// Calcula a dist�ncia a ser percorrida neste frame
	float TargetDistance = FMath::Min(ControlledPawn->RobotProperties.Speed * GetWorld()->GetDeltaSeconds(), EndDistance - CurrentDistance);
	CurrentDistance += TargetDistance;

	// Obt�m a nova posi��o (mantendo a Z atual) e rota��o (ignorando Z)
	FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	NewLocation.Z = CurrentLocation.Z; // Mant�m a coordenada Z atual

	FRotator NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	NewRotation.Pitch = 0.0f; // Opcional: for�a o Pitch a zero para evitar rota��o em Z

	// Move o ator
	ControlledPawn->SetActorLocation(NewLocation);
	ControlledPawn->SetActorRotation(NewRotation);

	// Atualiza o �ndice atual se necess�rio
	float NextDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(CurrentPathIndex + 1);
	if (CurrentDistance >= NextDistance && CurrentPathIndex < EndIndex) {
		CurrentPathIndex++;
	}

	return false; // Ainda em movimento
}



