
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
        CurrentPathIndex = 0;
        return true; // Movimento completo
    }

    // Obter a localização do ponto atual na spline
    FVector NewLocation = Spline->GetLocationAtSplinePoint(CurrentPathIndex, ESplineCoordinateSpace::World);
    FRotator TargetRotation = Spline->GetRotationAtSplinePoint(CurrentPathIndex, ESplineCoordinateSpace::World);

    if (MoveToNewLocation(NewLocation)) {
        // Se entrar aqui, entao já está no destino, incrementar imediatamente
        CurrentPathIndex++;
    }

    // Aplicar rotação suave enquanto o movimento está ocorrendo
    FRotator CurrentRotation = ControlledPawn->GetActorRotation();
    FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 2.0f); // Velocidade de interpolação
    ControlledPawn->SetActorRotation(SmoothedRotation);

    // Retornar false até que o movimento esteja completo
    return false;
}

void ARobotController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
    Super::OnMoveCompleted(RequestID, Result);

    // Se o movimento foi concluído com sucesso, incrementa o índice
    if (Result.Code == EPathFollowingResult::Success) {
        CurrentPathIndex++;
    }
}

