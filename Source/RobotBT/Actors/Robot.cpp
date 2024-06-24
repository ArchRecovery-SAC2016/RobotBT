#include "Robot.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "RobotBT/Controllers/RobotController.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;

}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 200;
}

void ARobot::GoInsideRoom(const FVector& RoomLocation) {
	ARobotController *RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	bool MoveCompleted = RobotController->MoveToNewLocation(RoomLocation);

	IsNotInsideRoom = !MoveCompleted;

}

bool ARobot::ProcessAction() {
	return true;
}

