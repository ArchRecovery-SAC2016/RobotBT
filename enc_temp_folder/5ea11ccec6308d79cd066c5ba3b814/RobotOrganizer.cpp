#include "RobotOrganizer.h"

#include "RobotBT/Controllers/RobotController.h"
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

void ARobotOrganizer::TaskFinished() {
	Super::TaskFinished();

	FurnitureToMoveLocation.Empty(); // clear the furniture to move location, so we can start a new task
}

bool ARobotOrganizer::MoveToFurniture() {
	if (GetRoom() == nullptr) return false;
	if (!FurnitureToMoveLocation.IsValidIndex(FurnitureToMoveLocationIndex)) return false;

	FVector CurrentFurnitureLocation = FurnitureToMoveLocation[FurnitureToMoveLocationIndex];
	IsMoving = true;

	bool IsAtLocation = GetRobotController()->MoveToNewLocation(CurrentFurnitureLocation);
	if (!IsAtLocation) {
		FurnitureToMoveLocationIndex++;
		IsMoving = false;
	}

	return IsAtLocation;
}

bool ARobotOrganizer::PlayMoveFurnitureAnimation() {
	// esperar um pouco e depois chamar 
	if (PlayMoveFurnitureAnimationFinished) return true;
	if (PlayMoveFurnitureAnimationIsRunning) return false; // ja esta rodando a animacao, nao precisa chamar de novo

	PlayMoveFurnitureAnimationIsRunning = true;

	float AnimationDuration = 2.0f;
	// Usa GetWorldTimerManager para iniciar um timer
	GetWorldTimerManager().SetTimer(
		MoveFurnitureAnimationTimerHandle, // FTimerHandle declarado na classe
		[this]() {
			// Lambda que roda quando o timer acaba
			PlayMoveFurnitureAnimationIsRunning = false;
			PlayMoveFurnitureAnimationFinished = true;
		},
		AnimationDuration, // Duração da animação
		false // Não looping
	);
	

	return false;
}

bool ARobotOrganizer::GetNextFurnitureToMove() {
	FurnitureToMoveLocationIndex++;

	if (!FurnitureToMoveLocation.IsValidIndex(FurnitureToMoveLocationIndex)) {
		TaskFinished();
		return false;
	}

	return true;

}

// here we need to get a different path from the cleaner
USplineComponent* ARobotOrganizer::GetRoomPath() {
	ARoomPreparation* PreparationRoom = Cast<ARoomPreparation>(GetRoom());
	if (PreparationRoom == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotOrganizer::GetRoomPath] Failed to get Room Path"));
		return nullptr;
	}

	return PreparationRoom->GetOrganizePath(PathIndex);
}

void ARobotOrganizer::StartNewTask(ESkillEnum SkillEnum, ARoom* Room) {
	Super::StartNewTask(SkillEnum, Room);
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