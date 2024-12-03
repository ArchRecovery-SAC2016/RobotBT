
#include "RobotController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "CADKernel/Math/BSpline.h"
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

void ARobotController::RotateToFaceActor(const AActor* ActorSelected) {
	if (ControlledPawn == nullptr) return;

	// e direction the enemy should move in
	const FVector PlayerDirection = ActorSelected->GetActorLocation() - ControlledPawn->GetActorLocation();
	const FRotator NewRotation = PlayerDirection.Rotation();
	//  Refresh the rotation to face the player
	ControlledPawn->SetActorRotation(FRotator(NewRotation.Pitch, NewRotation.Yaw, 0.0f));
}


bool ARobotController::MoveAlongSpline(USplineComponent* Spline, int32 StartIndex, int32 EndIndex) {
    if (!Spline || !ControlledPawn) {
        UE_LOG(LogTemp, Warning, TEXT("SplineComponent or ControlledPawn is null!"));
        return false;
    }

    // Certifique-se de que os índices estão dentro dos limites da spline
    if (StartIndex < 0 || EndIndex >= Spline->GetNumberOfSplinePoints() || StartIndex > EndIndex) {
        UE_LOG(LogTemp, Warning, TEXT("Invalid spline indices!"));
        return false;
    }

    // Verificar se já alcançamos o índice final
    if (CurrentPathIndex > EndIndex) {
        return true;
    }

    // Obter a localização do ponto atual na spline
    FVector NewLocation = Spline->GetLocationAtSplinePoint(CurrentPathIndex, ESplineCoordinateSpace::World);

    // Mover o pawn para a nova localização
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NewLocation);

    // Verificar se o pawn alcançou a localização
    float DistanceToTarget = FVector::Dist(ControlledPawn->GetActorLocation(), NewLocation);

    // Considera que alcançou o ponto se a distância for menor que um limiar
    const float ReachThreshold = 100.0f; // Tolerância para considerar que chegou
    if (DistanceToTarget <= ReachThreshold) {
        // Avançar para o próximo índice
        CurrentPathIndex++;

        // Se atingiu o último índice, retornar true
        if (CurrentPathIndex > EndIndex) {
            return true;
        }
    }

    // Ainda não terminou
    return false;
}

