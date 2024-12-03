#include "RobotOrganizer.h"

#include "RobotCleaner.h"
#include "RobotBT/Util/UtilMethods.h"

ARobotOrganizer::ARobotOrganizer() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobotOrganizer::BeginPlay() {
	Super::BeginPlay();
}

void ARobotOrganizer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool ARobotOrganizer::TaskExecution() {
	Super::TaskExecution();

	// sanitize and cleaning, just need to move along path
	if (TaskAllocated == ESkillEnum::MOVE_FURNITURE) {
		return MoveAlongPath();
	}

	return false;
}

USplineComponent* ARobotOrganizer::GetRoomPath() {
	ARoomPreparation* PreparationRoom = Cast<ARoomPreparation>(GetRoom());
	if (PreparationRoom == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotOrganizer::GetRoomPath] Failed to get Room Path"));
		return nullptr;
	}

	return PreparationRoom->GetOrganizePath(PathIndex);
}

void ARobotOrganizer::SetRoom(ARoom* NewRoomInstance) {
	if (ARoomPreparation* PrepRoom = Cast<ARoomPreparation>(NewRoomInstance)) {
		RoomPreparation = PrepRoom;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("SetRoom failed: RoomInstance is not an ARoomPreparation."));
	}
}

void ARobotOrganizer::StartNewTask(ESkillEnum SkillEnum, ARoom* Room) {
	Super::StartNewTask(SkillEnum, Room);

	ARoomPreparation* RoomInstance = Cast<ARoomPreparation>(Room);
	if (RoomInstance == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Room not found!"));
		return;
	}

	SetRoom(Room);
}

void ARobotOrganizer::GenerateRandomProperties() {
	Super::GenerateRandomProperties();

	TArray<FSkill> MoveSkill{
	{ ESkillEnum::NONE, 0.01f, 0.1f }, // adiciono um none. Servira como se nao tivesse essa skill
	{ ESkillEnum::MOVE_FURNITURE, 0.01f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.02f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.03f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.06f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.75f, 0.01f },
	{ ESkillEnum::MOVE_FURNITURE, 0.1f, 0.01f },

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