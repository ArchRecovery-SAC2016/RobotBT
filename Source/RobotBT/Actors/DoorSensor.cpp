
#include "DoorSensor.h"

#include "FurniturePlace.h"
#include "Components/BoxComponent.h"

ADoorSensor::ADoorSensor() {
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(BaseMesh);
}

void ADoorSensor::BeginPlay() {
	Super::BeginPlay();
}

void ADoorSensor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	IsPrepared();
}

bool ADoorSensor::IsPrepared() {
	if (CheckIsRooomClean() && CheckFornitureIsArranged()) {
		ChangeColor(true);
		return true;
	}

	return false;
}

FVector3d ADoorSensor::GetNextClearPosition() {
	FVector3d NextPosition = FVector3d(0, 0, 0);
	for (ARoomTrash* Trash : RoomTrash) {
		if (Trash->IsTrashClean == false) {
			return  Trash->GetActorLocation();
		}
	}

	return NextPosition;
}

FVector3d ADoorSensor::GetNextForniturePosition() {
	FVector3d NextPosition = FVector3d(0, 0, 0);
	for (AFurniturePlace* FurniturePlace : FurnituresPlace) {
		if (FurniturePlace->FurnitureInPlace == false) {
			return  FurniturePlace->GetActorLocation();
		}
	}

	return NextPosition;
}

void ADoorSensor::ChangeColor(bool NewValue) {
	if (BaseMesh == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No BaseMesh set for %s"), *GetName());
		return;
	}

	if (NewValue) {
		BaseMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(GreenColor));
	} else {
		BaseMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(RedColor));
	}
}

bool ADoorSensor::CheckFornitureIsArranged() {
	bool AllFurnitureRearranged = true;

	for (AFurniturePlace* FurniturePlace : FurnituresPlace) {
		if (FurniturePlace->FurnitureInPlace == false) {
			AllFurnitureRearranged = false;
			break;
		}
	}

	return AllFurnitureRearranged;
}

bool ADoorSensor::CheckIsRooomClean() {
	bool AllTrashRemoved = true;

	for (ARoomTrash* Trash : RoomTrash) {
		if (Trash->IsTrashClean == false) {
			AllTrashRemoved = false;
			break;
		}
	}

	return AllTrashRemoved;
}