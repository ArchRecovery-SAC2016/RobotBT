#include "RobotOrganizer.h"
#include "RobotBT/Controllers/RobotController.h"

ARobotOrganizer::ARobotOrganizer() {
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Organizer"));

}

void ARobotOrganizer::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);



}

void ARobotOrganizer::ProcessAction() {

	return;
}


