#include "Furniture.h"
#include "Components/BoxComponent.h"

AFurniture::AFurniture() {
	PrimaryActorTick.bCanEverTick = true;

	BaseLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Root Component"));
	BaseLocation->SetupAttachment(RootComponent);

	FurnitureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Furniture Mesh"));
	FurnitureMesh->SetupAttachment(BaseLocation);

	CorrectLocationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Correct Location Mesh"));
	CorrectLocationMesh->SetupAttachment(BaseLocation);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(FurnitureMesh);
}

void AFurniture::BeginPlay() {
	Super::BeginPlay();

    if (Collision) {
        Collision->OnComponentBeginOverlap.AddDynamic(this, &AFurniture::OnOverlapBegin);
    }
}

void AFurniture::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (ShouldMove) {
		MoveToCorrectLocation(DeltaTime);
	}
}

void AFurniture::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	// marca que coletou o lixo e desaparece com o mesh
	if (OtherActor->ActorHasTag("Organizer")) {
		ShouldMove = true;
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AFurniture::SetInPlace(bool NewValue) {
	if (NewValue == true) {
		FurnitureMesh->SetWorldLocationAndRotation(CorrectLocationMesh->GetComponentLocation(), CorrectLocationMesh->GetComponentRotation());
		CorrectLocationMesh->SetVisibility(false);
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InPlace = true;
		ChangeColor();
		ShouldMove = false;
	} else {
		// nao preciso fazer nada pq os moveis jah estao posicionados de forma incorreta e com a colisao habilidade
		InPlace = false;
	}
}

void AFurniture::ChangeColor() {
	if (InPlace) {
		FurnitureMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(GreenColor));
	} else {
		FurnitureMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(RedColor));
	}
}

void AFurniture::MoveToCorrectLocation(float DeltaTime) {
	// Get current and target transforms
	FVector CurrentLocation = FurnitureMesh->GetComponentLocation();
	FRotator CurrentRotation = FurnitureMesh->GetComponentRotation();

	FVector TargetLocation = CorrectLocationMesh->GetComponentLocation();
	FRotator TargetRotation = CorrectLocationMesh->GetComponentRotation();

	// Smoothly interpolate location and rotation
	FVector SmoothedLocation = FMath::Lerp(CurrentLocation, TargetLocation, DeltaTime * 10);
	FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10);

	// Apply interpolated values
	FurnitureMesh->SetWorldLocation(SmoothedLocation);
	FurnitureMesh->SetWorldRotation(SmoothedRotation);

	// Check if the mesh is "close enough" to the target
	float LocationTolerance = 1.0f;  // Set a small tolerance for position
	float RotationTolerance = 1.0f; // Set a small tolerance for rotation (degrees)

	if (FVector::Dist(SmoothedLocation, TargetLocation) <= LocationTolerance &&
		FMath::Abs(SmoothedRotation.Pitch - TargetRotation.Pitch) <= RotationTolerance &&
		FMath::Abs(SmoothedRotation.Yaw - TargetRotation.Yaw) <= RotationTolerance &&
		FMath::Abs(SmoothedRotation.Roll - TargetRotation.Roll) <= RotationTolerance) {
		SetInPlace(true); // Call to finish the placment
	}
}


