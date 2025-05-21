#include "Room.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "RobotBT/Util/UtilMethods.h"

ARoom::ARoom() {
	PrimaryActorTick.bCanEverTick = true;

	BaseLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Base Location"));
	BaseLocation->SetupAttachment(RootComponent);

	MainPath = CreateDefaultSubobject<USplineComponent>(TEXT("Main Path"));
	MainPath->SetupAttachment(BaseLocation);
}

void ARoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ARoom::BeginPlay() {
	Super::BeginPlay();

	OpenDoor(DoorOpened);
}

void ARoom::OpenDoor(bool NewValue) {
	DoorOpened = NewValue;

	AnimateOpenDoor(NewValue);
}

void ARoom::ChangeColorToOK(bool NewValue) {

}

USplineComponent* ARoom::GetRoomPath() {
	if (MainPath == nullptr) UUtilMethods::ShowLogMessage(TEXT("FAILED TO GET ROOM PATH	"), EMessageColorEnum::ERROR);

	return MainPath;
}

FVector ARoom::GetDoorEntrance() {
	if (GetRoomPath() == nullptr) {
		return FVector(0,0,0);
	}

	return GetRoomPath()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
}