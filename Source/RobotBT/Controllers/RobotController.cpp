
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

	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	FVector Direction = (NewPositionVector - CurrentLocation).GetSafeNormal();
	float Distance = FVector::Dist(CurrentLocation, NewPositionVector);

	// Verificar se já estamos próximos o suficiente do destino
	if (Distance < 50.0f) {
		return true;  // Alcançou o destino
	}

	// Calcular a distância a se mover neste frame
	float MoveDistance = ControlledPawn->Speed * DeltaTime;

	// Calcula a rotação necessária para apontar para a localização
	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0.0f; // Remove a inclinação no eixo X
	TargetRotation.Roll = 0.0f;  // Remove a rotação no eixo Y

	// Atualiza a rotação do robô gradualmente
	FRotator CurrentRotation = ControlledPawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5);

	ControlledPawn->SetActorRotation(NewRotation);

	// Verificar se a distância a ser percorrida neste frame é maior que a distância restante
	if (MoveDistance >= Distance) {
		// Mover diretamente para a localização final
		ControlledPawn->SetActorLocation(NewPositionVector);
		return true;  // Alcançou o destino
	}
	else {
		// Mover parcialmente na direção do destino
		FVector NewLocation = CurrentLocation + Direction * MoveDistance;
		ControlledPawn->SetActorLocation(NewLocation);
		return false;  // Ainda não alcançou o destino
	}
}

bool ARobotController::MoveAlongSpline(USplineComponent* SplineComponent, int32 StartIndex, int32 EndIndex, float DeltaTime) {
	if (SplineComponent == nullptr || ControlledPawn == nullptr) {
		return false;
	}

	// Inicializa as variáveis somente na primeira chamada
	if (CurrentPathIndex == -1) {
		CurrentPathIndex = StartIndex;
		CurrentDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(CurrentPathIndex);
	}

	// Verifica se já chegou ao final
	float EndDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(EndIndex);
	if (CurrentDistance >= EndDistance) {
		CurrentPathIndex = -1;  // Reset para a próxima chamada
		CurrentDistance = 0.0f; // Garante que a distância também seja resetada
		return true;            // Movimento concluído
	}

	// Calcula a distância a ser percorrida neste frame
	float TargetDistance = FMath::Min(ControlledPawn->Speed * DeltaTime, EndDistance - CurrentDistance);
	CurrentDistance += TargetDistance;

	// Obtém a nova posição e rotação
	FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	FRotator NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

	// Move o ator
	ControlledPawn->SetActorLocation(NewLocation);
	ControlledPawn->SetActorRotation(NewRotation);

	// Atualiza o índice atual se necessário
	float NextDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(CurrentPathIndex + 1);
	if (CurrentDistance >= NextDistance && CurrentPathIndex < EndIndex) {
		CurrentPathIndex++;
	}

	return false; // Ainda em movimento
}


