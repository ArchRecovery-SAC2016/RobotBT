#include "RobotCleaner.h"
#include "RobotBT/Controllers/RobotController.h"

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

void ARobotCleaner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);


}