#include "RoomTrash.h"
#include "Robot.h"
#include "Components/BoxComponent.h"

ARoomTrash::ARoomTrash() {
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(BaseMesh);
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

void ARoomTrash::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	ARobot* Robot = Cast<ARobot>(OtherActor);

	// marca que coletou o lixo e desaparece com o mesh
	if (Robot != nullptr) {
		IsTrashClean = true;
		BaseMesh->SetVisibility(false);
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
