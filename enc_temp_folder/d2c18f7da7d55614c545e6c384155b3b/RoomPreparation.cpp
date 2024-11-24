#include "RoomPreparation.h"

#include "Components/SplineComponent.h"

ARoomPreparation::ARoomPreparation() {
	PrimaryActorTick.bCanEverTick = true;
	
	OrganizePath1 = CreateDefaultSubobject<USplineComponent>(TEXT("Organize Path 1"));
	OrganizePath1->SetupAttachment(BaseLocation);

	OrganizePath2 = CreateDefaultSubobject<USplineComponent>(TEXT("Organize Path 2"));
	OrganizePath2->SetupAttachment(BaseLocation);
}

void ARoomPreparation::BeginPlay() {
	Super::BeginPlay();

}

void ARoomPreparation::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsRoomPrepared()) {
		ChangeColorToOK(true);
	} else {
		ChangeColorToOK(false);
	}
}

bool ARoomPreparation::IsRoomPrepared() {
	if (!IsTrashClean()) return false;
	if (!IsFurnitureOrganized()) return false;

	return true;
}

USplineComponent* ARoomPreparation::GetOrganizePath(int32 index) {
	if (index == 1) {
		if (OrganizePath1 == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("[ARoomPreparation::GetOrganizePath] Failed to get OrganizePath1"));
		}
		return OrganizePath1;
	}

	if (OrganizePath2 == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARoomPreparation::GetOrganizePath] Failed to get OrganizePath2"));
	}

	return OrganizePath2;
}

void ARoomPreparation::Initiate(FWorldRoomDataStruct RoomData) {
	for (ARoomTrash* Trash : Trashes) {
		Trash->SetIsTrashClean(RoomData.bIsClean);
	}

	for (AFurniture* Furniture : Furnitures) {
		Furniture->SetInPlace(RoomData.bIsPrepared);
	}
	
	OpenDoor(RoomData.bDoorOpen);
}

bool ARoomPreparation::IsTrashClean()	{
	for (ARoomTrash* Trash : Trashes) {
		if (!Trash->IsTrashClean) return false;
	}

	return true;
}

bool ARoomPreparation::IsFurnitureOrganized() {
	for (AFurniture* Furniture : Furnitures) {
		if (!Furniture->InPlace) return false;
	}

	return true;
}
