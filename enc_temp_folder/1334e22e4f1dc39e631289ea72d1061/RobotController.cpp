
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

bool ARobotController::MoveToNewLocation(const FVector& NewPositionVector, float DeltaTime) {
	if (ControlledPawn == nullptr) return false;

	// Obtem a localiza��o atual do ator
	FVector CurrentLocation = ControlledPawn->GetActorLocation();

	// Preserva a coordenada Z do rob� para ignorar mudan�as no eixo Z
	FVector AdjustedTargetLocation = NewPositionVector;
	AdjustedTargetLocation.Z = CurrentLocation.Z;

	// Calcula a dire��o no plano X-Y
	FVector Direction = (AdjustedTargetLocation - CurrentLocation).GetSafeNormal2D();

	// Calcula a dist�ncia no plano X-Y
	float Distance2D = FVector::Dist2D(CurrentLocation, AdjustedTargetLocation);

	// Verificar se j� estamos pr�ximos o suficiente no plano X-Y
	if (Distance2D < 50.0f) {
		return true; // Alcan�ou o destino
	}

	// Calcular a dist�ncia a se mover neste frame
	float MoveDistance = ControlledPawn->Speed * DeltaTime;

	// Calcula a rota��o necess�ria para apontar para a localiza��o (ignora Z)
	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0.0f; // Remove a inclina��o no eixo X
	TargetRotation.Roll = 0.0f;  // Remove a rota��o no eixo Y

	// Atualiza a rota��o do rob� gradualmente
	FRotator CurrentRotation = ControlledPawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5);

	ControlledPawn->SetActorRotation(NewRotation);

	// Verificar se a dist�ncia a ser percorrida neste frame � maior que a dist�ncia restante
	if (MoveDistance >= Distance2D) {
		// Mover diretamente para a localiza��o final (mantendo Z)
		ControlledPawn->SetActorLocation(AdjustedTargetLocation);
		return true; // Alcan�ou o destino
	}
	else {
		// Mover parcialmente na dire��o do destino (mantendo Z)
		FVector NewLocation = CurrentLocation + Direction * MoveDistance;
		NewLocation.Z = CurrentLocation.Z; // Mant�m a coordenada Z atual
		ControlledPawn->SetActorLocation(NewLocation);
		return false; // Ainda n�o alcan�ou o destino
	}
}


bool ARobotController::MoveAlongSpline(USplineComponent* SplineComponent, int32 StartIndex, int32 EndIndex, float DeltaTime) {
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
	float TargetDistance = FMath::Min(ControlledPawn->Speed * DeltaTime, EndDistance - CurrentDistance);
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



