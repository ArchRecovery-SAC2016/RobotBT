#include "RobotCleaner.h"

#include "RobotBT/RobotBTGameMode.h"

ARobotCleaner::ARobotCleaner() {
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("CleanerRobot"));
}

void ARobotCleaner::BeginPlay() {
	Super::BeginPlay();

	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

}

bool ARobotCleaner::ProcessAction() {

	return true;
}



bool ARobotCleaner::CleanRoom(ADoorSensor* RoomSelected) {
	// first is to move inside the room
	if (bGoInsideRoom == false) {
		GoInsideRoom(RoomSelected->InsideRoomLocation);
		return false;
	}
	
	if (RobotController == nullptr) return false;

	if (RoomSelected == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::CleanRoom] RoomSelected is nullptr"));
		return false;
	}

	bool AllClean = true;

	for (auto &RoomTrash : RoomSelected->RoomTrash) {
		
		if (RoomTrash->IsTrashClean == false) {
			bool AtLocation  = RobotController->MoveToActorLocation(RoomTrash);
			if (AtLocation) {
				ProcessAction();
			} else {
				AllClean = false;
			}
			
		}
	}

	if (AllClean == true && bGoOutsideRoom == false) {
		GoOutsideRoom(RoomSelected->OutsideRoomLocation);
		return false;
	}

	// if gets here, all the trash is clean. So we need to go to the door
	return AllClean;
}



/*
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
*/

