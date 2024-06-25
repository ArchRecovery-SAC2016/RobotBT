#include "RobotCleaner.h"

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

void ARobotCleaner::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (IsCleaning) {
		CleanRoom();
	} else if (IsOpeningDoor) {
		OpenRoom();
	} else if (IsSanitizing) {
		SanitizeRoom();
	}
}

void ARobotCleaner::StartCleaningRoom(ADoorSensor* _RoomSelected) {
	if (_RoomSelected == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] RoomSelected is nullptr"));
		return;
	}

	IsCleaning = true;
	this->RoomSelected = _RoomSelected;
}

void ARobotCleaner::StartSanitize(ADoorSensor* _RoomSelected) {
	if (_RoomSelected == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] RoomSelected is nullptr"));
		return;
	}

	IsSanitizing = true;
	this->RoomSelected = _RoomSelected;
}

void ARobotCleaner::StartOpeningDoor(ADoorSensor* _RoomSelected) {
	if (_RoomSelected == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] RoomSelected is nullptr"));
		return;
	}

	IsOpeningDoor = true;
	this->RoomSelected = _RoomSelected;
	this->RoomSelected->OnDoorOpen.AddDynamic(this, &ARobotCleaner::DoorOpenCompleted);
}

void ARobotCleaner::CleanRoom() {
	// first we need to move inside the room
	if (bGoInsideRoom == false) {
		GoInsideRoom(RoomSelected->InsideRoomLocation);
	}

	// if get here, we are inside the room,and now we need to clean the trash
	bool AllClean = true;
	for (auto &RoomTrash : RoomSelected->RoomTrash) {
		if (RoomTrash->IsTrashClean == false) {
			bool AtLocation  = RobotController->MoveToActorLocation(RoomTrash);
			if (AtLocation == false) {
				AllClean = false;
			}
		}
	}

	// if everything is clean, we need to go outside the room
	if (AllClean == true && bGoOutsideRoom == false) {
		GoOutsideRoom(RoomSelected->OutsideRoomLocation);
	}

	// if all clean and we are outside room, we finished here and notifi the success
	if (AllClean == true && bGoOutsideRoom == true) {
		IsCleaning = false;

		IsRobotSanitized = false;
		OnRoomCleaned.Broadcast(true);
	}
}

void  ARobotCleaner::SanitizeRoom() {
	if (bGoInsideRoom == false) {
		GoInsideRoom(RoomSelected->InsideRoomLocation);
	}

	// if is already inside, go outside
	if (bGoInsideRoom == true && bGoOutsideRoom == false) {
		GoOutsideRoom(RoomSelected->OutsideRoomLocation);
	}

	// if is already outside and inside, we are done
	if (bGoInsideRoom && bGoOutsideRoom) {
		IsSanitizing = false;
		OnRobotSanitized.Broadcast(true);
	}
}

void ARobotCleaner::OpenRoom() {
	// just try to get inside. When we touch the door, the door will open and the event DoorOpenCompleted will broadcast the success of this action
	if (bGoInsideRoom == false) {
		GoInsideRoom(RoomSelected->InsideRoomLocation);
	}

}

void ARobotCleaner::DoorOpenCompleted(bool bNewState) {
	IsOpeningDoor = false;

	OnDoorOpened.Broadcast(true);
}

