#include "RobotOrganizer.h"

ARobotOrganizer::ARobotOrganizer() {
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Organizer"));

}

bool ARobotOrganizer::ProcessAction() {

	return true;
}
