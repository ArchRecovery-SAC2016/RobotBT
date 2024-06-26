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

void ARobot::GoFrontOfRoom(const FVector& RoomLocation) {
	ARobotController *RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::GoFrontOfRoom] RobotController is nullptr"));
	}

	bFrontOfRoom = RobotController->MoveToNewLocation(RoomLocation);
}

void ARobot::GoCenterOfRoom(const FVector& RoomLocation) {
	ARobotController* RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::GoCenterOfRoom] RobotController is nullptr"));
	}

	bCenterOfRoom = RobotController->MoveToNewLocation(RoomLocation);

}

void ARobot::GoOutsideOfRoom(const FVector& RoomLocation) {
	ARobotController* RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::GoOutsideOfRoom] RobotController is nullptr"));
	}

	bOutsideRoom = RobotController->MoveToNewLocation(RoomLocation);

}

bool ARobot::ProcessAction() {
	return true;
}

