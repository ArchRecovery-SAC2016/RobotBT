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
	OnTaskFinished.Broadcast(RobotProperties);
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
