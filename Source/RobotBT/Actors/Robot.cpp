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
	if (GetRobotController() == nullptr) return;
	
	bFrontOfRoom = RobotController->MoveToNewLocation(RoomLocation);
}

void ARobot::GoCenterOfRoom(const FVector& RoomLocation) {
	if (GetRobotController() == nullptr) return;

	bCenterOfRoom = RobotController->MoveToNewLocation(RoomLocation);
}

void ARobot::GoOutsideOfRoom(const FVector& RoomLocation) {
	if (GetRobotController() == nullptr) return;

	bOutsideRoom = RobotController->MoveToNewLocation(RoomLocation);
}

bool ARobot::ProcessAction() {
	return true;
}

ARobotController* ARobot::GetRobotController() {
	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	return RobotController;
}

