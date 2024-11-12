#include "RobotCleaner.h"

#include "Room.h"
#include "RobotBT/Controllers/RobotController.h"

ARobotCleaner::ARobotCleaner() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobotCleaner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsSanitizing) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			MoveToRoomLocation(DeltaTime);
		} else {
			if (IsFinishedMovingAlongPath == false) {
				MoveAlongPath(DeltaTime);
			} else {
				IsSanitizing = false;
				OnTaskFinished.Broadcast();
			}
		}
	}

	if (IsOpeningDoor) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			MoveToRoomLocation(DeltaTime);
		} else {
			CurrentRoomInstace->OpenDoor(true);
			OnTaskFinished.Broadcast();
		}
	}

	if (IsCleaning) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			MoveToRoomLocation(DeltaTime);
		} else {
			if (IsFinishedMovingAlongPath == false) {
				MoveAlongPath(DeltaTime);
			}
			else {
				IsCleaning = false;
				OnTaskFinished.Broadcast();
			}
		}
	}
}

void ARobotCleaner::StartSanitizationTask(ARoom* Room) {
	IsSanitizing = true;

	// primeira coisa que faco eh preparar o robo para a tarefa
	CurrentRoomInstace = Room;
	IsAtRoomLocation = false;
}

void ARobotCleaner::StartOpenDoorTask(ARoom* Room) {
	IsOpeningDoor = true;

	CurrentRoomInstace = Room;
	IsAtRoomLocation = false;
}

void ARobotCleaner::StartCleaninTask(ARoom* Room) {
	IsCleaning = true;

	CurrentRoomInstace = Room;
	IsAtRoomLocation = false; // TODO: create a method to check if the robot is at the location, because if the robot is already at the location, it will not move
}
 
void ARobotCleaner::BeginPlay() {
	Super::BeginPlay();

}
