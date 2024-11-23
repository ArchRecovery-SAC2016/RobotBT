#include "RobotCleaner.h"

#include "RobotOrganizer.h"
#include "RoomPreparation.h"
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

	TaskFinishedWithSuccess();
	UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::INFO);
}

void ARobotCleaner::ExecuteTask(ESkillEnum SkillEnum, ARoom* Room) {
	Super::ExecuteTask(SkillEnum, Room);

	ARoomPreparation* RoomInstance = Cast<ARoomPreparation>(Room);
	if (RoomInstance == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Room not found!"));
		return;
	}

	if (SkillEnum == ESkillEnum::CLEAN_ROOM) {
		StartCleaninTask(Cast<ARoomPreparation>(Room));
	} else if (SkillEnum == ESkillEnum::OPEN_DOOR) {
		StartOpenDoorTask(Cast<ARoomPreparation>(Room));
	} else if (SkillEnum == ESkillEnum::SANITIZE_ROBOT) {
		StartSanitizationTask(Cast<ARoomPreparation>(Room));
	} else {
		UUtilMethods::PrintFailureMessage(EFailureReasonEnum::UnknownReason, RobotProperties);
	}
}

void ARobotCleaner::GenerateRandomProperties() {
	Super::GenerateRandomProperties();

	TArray<FSkill> CleanSkill {
	{ ESkillEnum::NONE, 0.01f, 0.1f }, // adiciono um none. Servira como se nao tivesse essa skill
	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.05f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.1f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.15f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.2f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.3f, 0.01f },

	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.03f },
	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.05f },
	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.07f },
	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.09f },
	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.12f }
	};
	int32 ClearSkillIndex = FMath::RandRange(0, CleanSkill.Num() - 1);
	RobotProperties.Skills.Add(CleanSkill[ClearSkillIndex]);

	TArray<FSkill> SanitizeSkill{
	{ ESkillEnum::NONE, 0.01f, 0.1f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.05f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.1f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.15f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.2f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.3f, 0.01f },

	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.03f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.05f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.07f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.09f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.12f }
	};
	int32 SanitizeSkillIndex = FMath::RandRange(0, SanitizeSkill.Num() - 1);
	RobotProperties.Skills.Add(SanitizeSkill[SanitizeSkillIndex]);

	TArray<FSkill> OpenDoorSkill {
	{ ESkillEnum::NONE, 0.01f, 0.1f },
	{ ESkillEnum::OPEN_DOOR, 0.01f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.05f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.1f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.15f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.2f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.3f, 0.01f },

	{ ESkillEnum::OPEN_DOOR, 0.01f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.01f, 0.03f },
	{ ESkillEnum::OPEN_DOOR, 0.01f, 0.05f },
	{ ESkillEnum::OPEN_DOOR, 0.01f, 0.07f },
	{ ESkillEnum::OPEN_DOOR, 0.01f, 0.09f },
	{ ESkillEnum::OPEN_DOOR, 0.01f, 0.12f }
	};
	int32 OpenDoorIndex = FMath::RandRange(0, OpenDoorSkill.Num() - 1);
	RobotProperties.Skills.Add(OpenDoorSkill[OpenDoorIndex]);
}


