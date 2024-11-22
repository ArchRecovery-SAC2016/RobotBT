#include "RobotOrganizer.h"

#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Util/UtilMethods.h"

ARobotOrganizer::ARobotOrganizer() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobotOrganizer::BeginPlay() {
	Super::BeginPlay();
}

void ARobotOrganizer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsOrganazing) {
		// first move to room location
		if (IsAtRoomLocation == false) {
			UpdateCurrentActionText("move-to-location");
			MoveToRoomLocation(DeltaTime);
		} else {
			if (IsFinishedMovingAlongPath == false) {
				UpdateCurrentActionText("move-furniture");
				MoveAlongPath(DeltaTime);
			} else {
				TaskFinished("Task move-furniture Finished");
				UpdateCurrentActionText("idle");
			}
		}
	}
}

void ARobotOrganizer::StartOrganizeTask(ARoomPreparation* Room) {
	if (Room == nullptr) return;
	SetRoom(Room);
	IsAtRoomLocation = false;
	IsOrganazing = true;
}

void ARobotOrganizer::TaskFinished(FString TaskMessage) {
	IsOrganazing = false;
	IsAtRoomLocation = false;
	IsFinishedMovingAlongPath = false;
	TaskFinishedWithSuccess();
	UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::INFO);
}

USplineComponent* ARobotOrganizer::GetRoomPath() {
	ARoomPreparation* PreparationRoom = Cast<ARoomPreparation>(GetRoom());
	if (PreparationRoom == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotOrganizer::GetRoomPath] Failed to get Room Path"));
		return nullptr;
	}

	return PreparationRoom->GetOrganizePath(PathIndex);
}

ARoom* ARobotOrganizer::GetRoom() {
	return RoomPreparation;
}

void ARobotOrganizer::SetRoom(ARoom* NewRoomInstance) {
	if (ARoomPreparation* PrepRoom = Cast<ARoomPreparation>(NewRoomInstance)) {
		RoomPreparation = PrepRoom;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("SetRoom failed: RoomInstance is not an ARoomPreparation."));
	}
}

void ARobotOrganizer::ExecuteTask(ESkillEnum SkillEnum, ARoom* Room) {
	Super::ExecuteTask(SkillEnum, Room);

	if (SkillEnum == ESkillEnum::MOVE_FURNITURE) {
		StartOrganizeTask(Cast<ARoomPreparation>(Room));
	}
}

void ARobotOrganizer::GenerateRandomProperties() {
	Super::GenerateRandomProperties();

	TArray<FSkill> MoveSkill{
	{ ESkillEnum::NONE, 0.01f, 0.1f }, // adiciono um none. Servira como se nao tivesse essa skill
	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.05f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.1f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.15f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.2f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.3f, 0.01f },

	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.03f },
	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.05f },
	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.07f },
	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.09f },
	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.12f }
	};
	int32 MoveSkillIndex = FMath::RandRange(0, MoveSkill.Num() - 1);
	RobotProperties.Skills.Add(MoveSkill[MoveSkillIndex]);
}
