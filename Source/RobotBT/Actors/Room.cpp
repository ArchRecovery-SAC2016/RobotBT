#include "Room.h"
#include "Components/BoxComponent.h"
#include "RobotBT/Util/UtilMethods.h"

ARoom::ARoom() {
	PrimaryActorTick.bCanEverTick = true;

	BaseLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Base Location"));
	BaseLocation->SetupAttachment(RootComponent);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(BaseLocation);

	DoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Door DoorCollision"));
	DoorCollision->SetupAttachment(DoorMesh);
}

void ARoom::BeginPlay() {
	Super::BeginPlay();

	ControlDoorOpen();
}

void ARoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	GetActionsFinished();
}

bool ARoom::GetActionsFinished() {
	if (ActionsFinished) {
		ChangeColor(true);
		return true;
	}

	ChangeColor(false);
	return false;
}

void ARoom::OpenDoor(bool NewValue) {
	DoorOpened = NewValue;
	ControlDoorOpen();
}

void ARoom::ChangeColor(bool NewValue) {
	if (DoorMesh == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No DoorMesh set for %s"), *GetName());
		return;
	}

	if (NewValue) {
		DoorMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(GreenColor));
	} else {
		DoorMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(RedColor));
	}
}

FVector ARoom::GetDoorEntrance() {
	if (Path == nullptr) {
		UUtilMethods::ShowLogMessage(TEXT("FAILED TO GET ROOM ENTRANCE"), EMessageColorEnum::ERROR);
		return FVector(0,0,0);
	}

	return Path->GetLocationByKey(0);
}

void ARoom::ControlDoorOpen() {
	if (DoorOpened) {
		DoorCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FVector Location = DoorMesh->GetComponentLocation();
		DoorMesh->SetWorldLocation(FVector(Location.X, Location.Y, -240)); // -240 will be near visible
	}else {
		DoorCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FVector Location = DoorMesh->GetComponentLocation();
		DoorMesh->SetWorldLocation(FVector(Location.X, Location.Y, 120));
	}
}
