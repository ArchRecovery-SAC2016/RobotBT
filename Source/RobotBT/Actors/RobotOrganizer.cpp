#include "RobotOrganizer.h"
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

void ARobotOrganizer::ProcessAction() {

	return;
}

void ARobotOrganizer::AddMoveTask(AFurniturePlace* FurniturePlace)	{
	FurnitureToMove.Add(FurniturePlace);
}

void ARobotOrganizer::StartMoveFurniture(ADoorSensor* Room) {
	
}

void ARobotOrganizer::MoveFurniture() {
	
}
