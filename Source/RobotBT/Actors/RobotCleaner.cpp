#include "RobotCleaner.h"

#include "RobotBT/RobotBTGameMode.h"

ARobotCleaner::ARobotCleaner() {
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Cleaner"));
}

bool ARobotCleaner::ProcessAction() {

	return true;
}

ADoorSensor* ARobotCleaner::GetNextRoom() {
	ARobotBTGameMode* GameMode = Cast<ARobotBTGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::GetNextRoom] Failed to recover the GameMode"));
		return nullptr;
	}

	for (ADoorSensor* Door : GameMode->GetDoors()) {
		if (Door->CheckIsRooomClean() == false) {
			return Door;
		}
	}

	return nullptr;
}


