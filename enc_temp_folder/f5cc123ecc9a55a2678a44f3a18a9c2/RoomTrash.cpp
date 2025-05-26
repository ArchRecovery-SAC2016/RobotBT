#include "RoomTrash.h"
#include "Components/BoxComponent.h"

ARoomTrash::ARoomTrash() {
	PrimaryActorTick.bCanEverTick = true;

	TrashMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	TrashMesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(TrashMesh);
}

void ARoomTrash::BeginPlay() {
	Super::BeginPlay();

	if (Collision) {
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ARoomTrash::OnOverlapBegin);
	}
}

void ARoomTrash::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ARoomTrash::SetIsTrashClean(bool NewValue) {
	if (NewValue) {
		IsTrashClean = true;
		TrashMesh->SetVisibility(false);
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	} else {
		IsTrashClean = true;
		TrashMesh->SetVisibility(true);
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ARoomTrash::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	// marca que coletou o lixo e desaparece com o mesh
	if (OtherActor->ActorHasTag("Cleaner")) {
		SetIsTrashClean(true);
	}
}
