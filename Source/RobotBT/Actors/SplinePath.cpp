#include "SplinePath.h"

#include "Components/SplineComponent.h"


ASplinePath::ASplinePath() {
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void ASplinePath::BeginPlay() {
	Super::BeginPlay();

	
}

void ASplinePath::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

FVector ASplinePath::GetLocationByKey(int32 KeyIndex) {
     if (KeyIndex != INDEX_NONE) {
        return Spline->GetLocationAtSplinePoint(KeyIndex, ESplineCoordinateSpace::World);
    }
    return FVector::ZeroVector; // Retorna uma localização nula se não encontrado
}

