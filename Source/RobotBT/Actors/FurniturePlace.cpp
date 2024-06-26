
#include "FurniturePlace.h"

#include "Furniture.h"
#include "Components/BoxComponent.h"

AFurniturePlace::AFurniturePlace() {
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(BaseMesh);
}

void AFurniturePlace::BeginPlay() {
	Super::BeginPlay();

	if (Collision) {
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AFurniturePlace::OnOverlapBegin);
		Collision->OnComponentEndOverlap.AddDynamic(this, &AFurniturePlace::OnOverlapEnd);
	}

	if (AcceptedFurniture == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No correct furniture set for %s"), *GetName());
	}

	// tem que comecar desabilitado. Pq se nao, o overlap acontece antes do tempo
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ativa a colisao apos 1 segundo
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, [this] { Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); }, 0.2f, false);


	ChangeColor(false);
}

void AFurniturePlace::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AFurniture* Furniture = Cast<AFurniture>(OtherActor);
	
	if (Furniture != nullptr && Furniture == AcceptedFurniture) {
		ChangeColor(true);
	}
}

void AFurniturePlace::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	ChangeColor(false);
}

void AFurniturePlace::ChangeColor(bool NewValue) {
	OnFurnitureStateChange.Broadcast(NewValue);

	FurnitureInPlace = NewValue;

	if (BaseMesh == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No BaseMesh set for %s"), *GetName());
		return;
	}

	if (FurnitureInPlace) {
		BaseMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(GreenColor));
	} else {
		BaseMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(RedColor));
	}
}



