#include "RobotCleaner.h"

#include "Room.h"
#include "RobotBT/Controllers/RobotController.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Util/UtilMethods.h"

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
				UUtilMethods::ShowLogMessage(TEXT("Task sanitize-robot Finished"), EMessageColorEnum::INFO);
			}
		}
	}

	if (IsOpeningDoor) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			MoveToRoomLocation(DeltaTime);
		} else {
			IsOpeningDoor = false;
			CurrentRoomInstace->OpenDoor(true);
			OnTaskFinished.Broadcast();
			UUtilMethods::ShowLogMessage(TEXT("Task open-door Finished"), EMessageColorEnum::INFO);
		}
	}

	if (IsCleaning) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			MoveToRoomLocation(DeltaTime);
		} else {
			if (IsFinishedMovingAlongPath == false) {
				MoveAlongPath(DeltaTime);
			} else {
				IsCleaning = false;
				OnTaskFinished.Broadcast();
				UUtilMethods::ShowLogMessage(TEXT("Task clean-room Finished"), EMessageColorEnum::INFO);
			}
		}
	}
}

void ARobotCleaner::StartSanitizationTask(ARoom* Room) {
	if (Room == nullptr) return;

	IsSanitizing = true;

	// primeira coisa que faco eh preparar o robo para a tarefa
	CurrentRoomInstace = Room;
	IsAtRoomLocation = false;
}

void ARobotCleaner::StartOpenDoorTask(ARoom* Room) {
	if (Room == nullptr) return;

	IsOpeningDoor = true;

	CurrentRoomInstace = Room;
	IsAtRoomLocation = false;
}

void ARobotCleaner::StartCleaninTask(ARoom* Room) {
	if (Room == nullptr) return;

	IsCleaning = true;

	CurrentRoomInstace = Room;
	IsAtRoomLocation = false; // TODO: create a method to check if the robot is at the location, because if the robot is already at the location, it will not move
}
 
void ARobotCleaner::BeginPlay() {
	Super::BeginPlay();

}
