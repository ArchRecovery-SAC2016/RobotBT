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
	if (bFrontOfRoom == false) {
		GoFrontOfRoom(RoomSelected->FrontRoomLocation);
	}

	// if get here, we are inside the room,and now we need to clean the trash
	bool AllClean = true;
	for (auto &RoomTrash : RoomSelected->RoomTrash) {
		if (RoomTrash->IsTrashClean == false) {
			bool AtLocation  = GetRobotController()->MoveToActorLocation(RoomTrash);
			if (AtLocation == false) {
				AllClean = false;
			}
		}
	}

	// if everything is clean, we need to go outside the room
	if (AllClean == true && bOutsideRoom == false) {
		GoOutsideOfRoom(RoomSelected->OutsideRoomLocation);
	}

	// if all clean and we are outside room, we finished here and notifi the success
	if (AllClean == true && bOutsideRoom == true) {
		IsCleaning = false;
		IsRobotSanitized = false;

		// reset the variables
		bFrontOfRoom = false;
		bCenterOfRoom = false;
		bOutsideRoom = false;

		OnRoomCleaned.Broadcast(true);

	}
}

void  ARobotCleaner::SanitizeRoom() {
	if (bFrontOfRoom == false) {
		GoFrontOfRoom(RoomSelected->FrontRoomLocation);
	}

	// if is already in front, and not in the center, go to the center
	if (bFrontOfRoom == true && bCenterOfRoom == false) {
		GoCenterOfRoom(RoomSelected->CenterRoomLocation);
	}

	// if is already in front, and not in the center, got to the center
	if (bFrontOfRoom == true && bCenterOfRoom == true && bOutsideRoom == false) {
		GoOutsideOfRoom(RoomSelected->OutsideRoomLocation);
	}

	// if is already outside and inside, we are done
	if (bOutsideRoom) {
		IsSanitizing = true;

		bFrontOfRoom = false;
		bCenterOfRoom = false;
		bOutsideRoom = false;

		OnRobotSanitized.Broadcast(true);
	}
}

void ARobotCleaner::OpenRoom() {
	// just try to get inside. When we touch the door, the door will open and the event DoorOpenCompleted will broadcast the success of this action
	if (bFrontOfRoom == false) {
		GoFrontOfRoom(RoomSelected->FrontRoomLocation);
	}

}

void ARobotCleaner::DoorOpenCompleted(bool bNewState) {
	IsOpeningDoor = false;

	OnDoorOpened.Broadcast(true);
}

