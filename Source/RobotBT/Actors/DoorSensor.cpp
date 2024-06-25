
#include "DoorSensor.h"

#include "FurniturePlace.h"
#include "Components/BoxComponent.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Util/UtilMethods.h"

ADoorSensor::ADoorSensor() {
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(BaseMesh);
}

void ADoorSensor::BeginPlay() {

	if (Collision) {
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ADoorSensor::OnOverlapBegin);
	}

	Super::BeginPlay();

	ControlDoorOpen();
}

void ADoorSensor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	IsPrepared();
}

bool ADoorSensor::IsPrepared() {
	if (CheckIsRooomClean() && CheckFornitureIsArranged()) {
		ChangeColor(true);
		return true;
	} else {
		ChangeColor(false);
	}

	return false;
}

FVector3d ADoorSensor::GetNextClearPosition() {
	FVector3d NextPosition = FVector3d(0, 0, 0);
	for (ARoomTrash* Trash : RoomTrash) {
		if (Trash->IsTrashClean == false) {
			return  Trash->GetActorLocation();
		}
	}

	return NextPosition;
}

FVector3d ADoorSensor::GetNextForniturePosition() {
	FVector3d NextPosition = FVector3d(0, 0, 0);
	for (AFurniturePlace* FurniturePlace : FurnituresPlace) {
		if (FurniturePlace->FurnitureInPlace == false) {
			return  FurniturePlace->GetActorLocation();
		}
	}

	return NextPosition;
}

void ADoorSensor::ChangeColor(bool NewValue) {
	if (BaseMesh == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No BaseMesh set for %s"), *GetName());
		return;
	}

	if (NewValue) {
		BaseMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(GreenColor));
	} else {
		BaseMesh->SetVectorParameterValueOnMaterials("BaseColor", FVector(RedColor));
	}
}

bool ADoorSensor::CheckFornitureIsArranged() {
	bool AllFurnitureRearranged = true;

	for (AFurniturePlace* FurniturePlace : FurnituresPlace) {
		if (FurniturePlace->FurnitureInPlace == false) {
			AllFurnitureRearranged = false;
			break;
		}
	}

	return AllFurnitureRearranged;
}

bool ADoorSensor::CheckIsRooomClean() {
	bool AllTrashRemoved = true;

	for (ARoomTrash* Trash : RoomTrash) {
		if (Trash->IsTrashClean == false) {
			AllTrashRemoved = false;
			break;
		}
	}

	return AllTrashRemoved;
}

void ADoorSensor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

void ADoorSensor::ControlDoorOpen() {
	if (Opened) {
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FVector Location = BaseMesh->GetComponentLocation();
		BaseMesh->SetWorldLocation(FVector(Location.X, Location.Y, -199)); // -199 will be near the floor
	}else {
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FVector Location = BaseMesh->GetComponentLocation();
		BaseMesh->SetWorldLocation(FVector(Location.X, Location.Y, 0));
	}
}
