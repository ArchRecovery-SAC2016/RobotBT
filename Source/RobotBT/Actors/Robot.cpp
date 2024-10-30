#include "Robot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RobotBT/Controllers/RobotController.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool ARobot::ExecuteNextMovePlan() {
	if (CurrentSplinePath == nullptr) return false;

	// Pega o caminho baseado no identificador atual
	CurrentSplinePath = GetPathByIndentifier(MovePlan[CurrentMovePlanIndex].Identifier);

	if (CurrentSplinePath == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::ExecuteCurrentMovePlan] CurrentPath is nullptr"));
		return false;
	}

	return true;
}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 200;
}

ARobotController* ARobot::GetRobotController() {
	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	return RobotController;
}

ASplinePath* ARobot::GetPathByIndentifier(FString Identifier) {
	for (auto& Path : SplinePath) {
		if (Path->Identifier == Identifier) {
			return Path;
		}
	}

	return nullptr;
}

void ARobot::OnCurrentMovePlanFinished() {
	if (CurrentMovePlanIndex + 1 < MovePlan.Num()) {
		ExecuteNextMovePlan();
	}
	else {
		CurrentSplinePath = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Movimento completo!"));
	}
}

void ARobot::ProcessAction() {

}
