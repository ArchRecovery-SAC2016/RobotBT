#include "RoomPreparation.h"

ARoomPreparation::ARoomPreparation() {
	PrimaryActorTick.bCanEverTick = true;

}

void ARoomPreparation::BeginPlay() {
	Super::BeginPlay();

}

void ARoomPreparation::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsRoomPrepared()) {
		ChangeColorToOK(true);
	}
}

bool ARoomPreparation::IsRoomPrepared() {
	for (ARoomTrash* Trash : Trashes) {
		if (!Trash->IsTrashClean) return false;
	}

	for (AFurniture* Furniture : Furnitures) {
		if (!Furniture->InPlance) return false;
	}


	return true;
}