#include "NavigableArea.h"

ANavigableArea::ANavigableArea() {
	
}

void ANavigableArea::HideRoof(bool NewValue) {
	if (RoofMeshes != nullptr) {
		RoofMeshes->SetVisibility(NewValue, true);
	}
}
void ANavigableArea::BeginPlay() {
	Super::BeginPlay();

}
