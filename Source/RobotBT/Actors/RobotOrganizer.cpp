#include "RobotOrganizer.h"

ARobotOrganizer::ARobotOrganizer() {
	PrimaryActorTick.bCanEverTick = true;
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
			}
			else {
				IsOrganazing = false;
				OnTaskFinished.Broadcast();
			}
		}
	}
}

void ARobotOrganizer::BeginPlay() {
	Super::BeginPlay();

}
