#include "NavigableArea.h"

ANavigableArea::ANavigableArea() {

	// Criação do componente raiz
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Inicializa o componente CenarioMeshes
	SceneStructure = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Structure"));
	SceneStructure->SetupAttachment(RootComponent);

	// Inicializa o componente RoofMeshes
	RoofStructure = CreateDefaultSubobject<USceneComponent>(TEXT("RoofMeshes"));
	RoofStructure->SetupAttachment(RootComponent);
}

void ANavigableArea::HideRoof(bool NewValue) {
	if (RoofStructure != nullptr) {
		RoofStructure->SetVisibility(NewValue, true);
	}
}
void ANavigableArea::BeginPlay() {
	Super::BeginPlay();

}
