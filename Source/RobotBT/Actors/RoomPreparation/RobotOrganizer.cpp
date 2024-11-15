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
			MoveToRoomLocation(DeltaTime);
		}
		else {
			if (IsFinishedMovingAlongPath == false) {
				MoveAlongPath(DeltaTime);
			} else {
				TaskFinished("Task move-furniture Finished");
			}
		}
	}
}

void ARobotOrganizer::StartOrganizeTask(ARoom* Room) {
	if (Room == nullptr) return;

	CurrentRoomInstace = Room;
	IsAtRoomLocation = false;

	IsOrganazing = true;
}



void ARobotOrganizer::TaskFinished(FString TaskMessage) {
	IsOrganazing = false;
	IsAtRoomLocation = false;
	IsFinishedMovingAlongPath = false;
	OnTaskFinished.Broadcast();
	UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::INFO);
}

