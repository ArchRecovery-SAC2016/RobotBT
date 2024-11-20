#include "RobotCleaner.h"

#include "RoomPreparation.h"
#include "RobotBT/Controllers/RobotController.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Util/UtilMethods.h"

ARobotCleaner::ARobotCleaner() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobotCleaner::BeginPlay() {
	Super::BeginPlay();
}

void ARobotCleaner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsSanitizing) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			UpdateCurrentActionText("move-to-location");
			MoveToRoomLocation(DeltaTime);
		} else {
			if (IsFinishedMovingAlongPath == false) {
				UpdateCurrentActionText("sanitize-robot");
				MoveAlongPath(DeltaTime);
			} else {
				TaskFinished("Task sanitize-robot Finished");
				UpdateCurrentActionText("idle");
			}
		}
	}

	if (IsOpeningDoor) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			UpdateCurrentActionText("move-to-location");
			MoveToRoomLocation(DeltaTime);
		} else {
			UpdateCurrentActionText("open-door");
			GetRoom()->OpenDoor(true);
			TaskFinished("Task open-door Finished");
		}
	}

	if (IsCleaning) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			UpdateCurrentActionText("move-to-location");
			MoveToRoomLocation(DeltaTime);
		} else {
			if (IsFinishedMovingAlongPath == false) {
				UpdateCurrentActionText("clean-room ");
				MoveAlongPath(DeltaTime);
			} else {
				TaskFinished("Task clean-room Finished");
				UpdateCurrentActionText("idle");
			}
		}
	}
}

void ARobotCleaner::StartSanitizationTask(ARoomPreparation* Room) {
	if (Room == nullptr) return;

	IsSanitizing = true;

	// primeira coisa que faco eh preparar o robo para a tarefa
	SetRoom(Room);
	IsAtRoomLocation = false;
}

void ARobotCleaner::StartOpenDoorTask(ARoomPreparation* Room) {
	if (Room == nullptr) return;

	IsOpeningDoor = true;

	SetRoom(Room);
	IsAtRoomLocation = false;
}

void ARobotCleaner::StartCleaninTask(ARoomPreparation* Room) {
	if (Room == nullptr) return;

	IsCleaning = true;

	SetRoom(Room);
	IsAtRoomLocation = false; // TODO: create a method to check if the robot is at the location, because if the robot is already at the location, it will not move
}

void ARobotCleaner::TaskFinished(FString TaskMessage) {
	IsSanitizing = false;
	IsAtRoomLocation = false;
	IsCleaning = false;
	IsOpeningDoor = false;
	IsFinishedMovingAlongPath = false;
	OnTaskFinished.Broadcast();
	UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::INFO);
}


