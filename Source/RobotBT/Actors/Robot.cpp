#include "Robot.h"

#include "GameFramework/CharacterMovementComponent.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;

}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 50;
}


TArray<ADoorSensor*> ARobot::GetRoomsToBePrepared() {
	TArray<ADoorSensor*> PreparedRooms;

	for (ADoorSensor* Door : DoorSensor) {
		if (Door->IsPrepared() == false) {
			PreparedRooms.Add(Door);
		}
	}

	return PreparedRooms;
}

ADoorSensor* ARobot::GetNextRoom() {

	return nullptr;
}

bool ARobot::ProcessAction() { return true; }

ADoorSensor* ARobot::GetNextRoomToBePrepared() {
	TArray<ADoorSensor*> PreparedRooms = GetRoomsToBePrepared();

	if (PreparedRooms.Num() > 0) {
		return PreparedRooms[0];
	}

	return nullptr;
}