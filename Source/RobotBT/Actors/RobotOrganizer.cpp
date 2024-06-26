#include "RobotOrganizer.h"

#include "FurniturePlace.h"
#include "RobotBT/Controllers/RobotController.h"

ARobotOrganizer::ARobotOrganizer() {
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Organizer"));

}

void ARobotOrganizer::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (IsMovingFurniture)	{
		MoveFurniture();
	}

}

bool ARobotOrganizer::ProcessAction() {

	return true;
}

void ARobotOrganizer::AddMoveTask(AFurniturePlace* FurniturePlace)	{
	FurnitureToMove.Add(FurniturePlace);
}

void ARobotOrganizer::StartMoveFurniture(ADoorSensor* Room) {
	if (Room == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] RoomSelected is nullptr"));
		return;
	}

	if (FurnitureToMove.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] No Furniture to move"));
		return;
	}

	IsMovingFurniture = true;
	this->RoomSelected = Room;
}

void ARobotOrganizer::MoveFurniture() {
	// first, move to the room
	if (bFrontOfRoom == false) {
		GoFrontOfRoom(RoomSelected->FrontRoomLocation);
	}

	bool AllFurnitureCorrect = true;
	for (auto FurniturePlace : FurnitureToMove) {
		if (FurniturePlace->AcceptedFurniture != nullptr && FurniturePlace->FurnitureInPlace == false) {
			bool AtLocation = GetRobotController()->MoveToActorLocation(FurniturePlace->AcceptedFurniture);
			if (AtLocation == false) {
				AllFurnitureCorrect = false;
			}
		}
	}

	// if everything is ok, we need to go outside the room
	if (AllFurnitureCorrect == true && bOutsideRoom == false) {
		GoOutsideOfRoom(RoomSelected->OutsideRoomLocation);
	}

	if (AllFurnitureCorrect) {
		bFrontOfRoom = false;

		IsMovingFurniture = false;
		OnFurnitureMoveEnded.Broadcast(true);
	}
}
