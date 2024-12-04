#include "RobotCleaner.h"

#include "RobotOrganizer.h"
#include "RoomPreparation.h"
#include "RobotBT/Util/UtilMethods.h"

ARobotCleaner::ARobotCleaner() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobotCleaner::BeginPlay() {
	Super::BeginPlay();
}

void ARobotCleaner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

bool ARobotCleaner::TaskExecution() {
	Super::TaskExecution();

	// sanitize and cleaning, just need to move along path
	if (TaskAllocated == ESkillEnum::SANITIZE_ROBOT || TaskAllocated == ESkillEnum::CLEAN_ROOM) {
		// will return true when finished
		return MoveAlongPath();
	}

	// open door, just open
	if (TaskAllocated == ESkillEnum::OPEN_DOOR) {
		GetRoom()->OpenDoor(true);
		return true;

	}

	return false;
}

// TODO: CHANGE THIS TO START_TASK!!!
void ARobotCleaner::StartNewTask(ESkillEnum SkillEnum, ARoom* Room) {
	Super::StartNewTask(SkillEnum, Room);

}

void ARobotCleaner::GenerateRandomProperties() {
	Super::GenerateRandomProperties();

	// reset the properties
	FCleanerProperties NewProperties;
	CleanerProperties = NewProperties;

	TArray<FSkill> CleanSkill {
	{ ESkillEnum::NONE, 0.01f, 0.1f }, // adiciono um none. Servira como se nao tivesse essa skill
	{ ESkillEnum::CLEAN_ROOM, 0.01f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.02f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.03f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.06f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.75f, 0.01f },
	{ ESkillEnum::CLEAN_ROOM, 0.1f, 0.01f },

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
	{ ESkillEnum::NONE, 1.0f, 0.1f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.01f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.02f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.03f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.06f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.75f, 0.01f },
	{ ESkillEnum::SANITIZE_ROBOT, 0.1f, 0.01f },

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
	{ ESkillEnum::OPEN_DOOR, 0.02f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.03f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.06f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.75f, 0.01f },
	{ ESkillEnum::OPEN_DOOR, 0.1f, 0.01f },

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


