
#include "Room.h"

#include "FurniturePlace.h"
#include "Components/BoxComponent.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Util/UtilMethods.h"

ARoom::ARoom() {
	PrimaryActorTick.bCanEverTick = true;

	BaseLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Base Location"));
	BaseLocation->SetupAttachment(RootComponent);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(BaseLocation);

	DoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Door DoorCollision"));
	DoorCollision->SetupAttachment(DoorMesh);
}

void ARoom::BeginPlay() {
	if (DoorCollision) {
		DoorCollision->OnComponentBeginOverlap.AddDynamic(this, &ARoom::OnOverlapBegin);
	}

	Super::BeginPlay();

	ControlDoorOpen();
}

void ARoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	IsPrepared();
}

bool ARoom::IsPrepared() {
	if (CheckIsRooomClean() && CheckFornitureIsArranged()) {
		ChangeColor(true);
		return true;
	}

	ChangeColor(false);
	return false;
}

FVector3d ARoom::GetNextClearPosition() {
	FVector3d NextPosition = FVector3d(0, 0, 0);
	for (ARoomTrash* Trash : RoomTrash) {
		if (Trash->IsTrashClean == false) {
			return  Trash->GetActorLocation();
		}
	}

	return NextPosition;
}

FVector3d ARoom::GetNextForniturePosition() {
	FVector3d NextPosition = FVector3d(0, 0, 0);
	for (AFurniturePlace* FurniturePlace : FurnituresPlace) {
		if (FurniturePlace->FurnitureInPlace == false) {
			return  FurniturePlace->GetActorLocation();
		}
	}

	return NextPosition;
}

void ARoom::ChangeColor(bool NewValue) {
	if (DoorMesh == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No DoorMesh set for %s"), *GetName());
		return;
	}

	if (NewValue) {
		DoorMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(GreenColor));
	} else {
		DoorMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(RedColor));
	}
}

bool ARoom::CheckFornitureIsArranged() {
	bool AllFurnitureRearranged = true;

	for (AFurniturePlace* FurniturePlace : FurnituresPlace) {
		if (FurniturePlace->FurnitureInPlace == false) {
			AllFurnitureRearranged = false;
			break;
		}
	}

	return AllFurnitureRearranged;
}

FVector ARoom::GetDoorEntrance() {
	if (Path == nullptr) {
		return FVector(0,0,0);
	}

	return Path->GetLocationByKey(0);
}

bool ARoom::CheckIsRooomClean() {
	bool AllTrashRemoved = true;

	for (ARoomTrash* Trash : RoomTrash) {
		if (Trash->IsTrashClean == false) {
			AllTrashRemoved = false;
			break;
		}
	}

	return AllTrashRemoved;
}

void ARoom::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	// if already open, we don't need to open it again
	if (Opened == true) return;

	// marca que coletou o lixo e desaparece com o mesh
	if (OtherActor->ActorHasTag("CleanerRobot")) {
		UUtilMethods::ShowLogMessage("Clean Robot opened a door", EMessageColorEnum::SUCCESS);
		Opened = true;
		OnDoorOpen.Broadcast(true);
		// will move the base mesh and change the collision
		ControlDoorOpen();
	}
}

void ARoom::ControlDoorOpen() {
	if (Opened) {
		DoorCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FVector Location = DoorMesh->GetComponentLocation();
		DoorMesh->SetWorldLocation(FVector(Location.X, Location.Y, -199)); // -199 will be near the floor
	}else {
		DoorCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FVector Location = DoorMesh->GetComponentLocation();
		DoorMesh->SetWorldLocation(FVector(Location.X, Location.Y, 0));
	}
}
