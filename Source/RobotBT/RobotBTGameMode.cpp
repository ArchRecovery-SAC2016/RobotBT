// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTGameMode.h"
#include "RobotBTPlayerController.h"
#include "RobotBTCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ARobotBTGameMode::ARobotBTGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARobotBTPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL) {
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}


	PrimaryActorTick.bCanEverTick = true;
}


void ARobotBTGameMode::BeginPlay() {
    Super::BeginPlay();

	// Load all Doors Sensors, so we can watch it
    TArray<AActor*> FoundSensors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoorSensor::StaticClass(), FoundSensors);

    for (AActor* Actor : FoundSensors) {
        ADoorSensor* Sensor = Cast<ADoorSensor>(Actor);
        if (Sensor != nullptr) {
        	DoorSensors.Add(Sensor);
		}
    }
}

void ARobotBTGameMode::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

   CheckDoors();
}

void ARobotBTGameMode::CheckDoors() {
	for (ADoorSensor* Sensor: DoorSensors) {
		if (GetWorldKnowledgeWidget() == nullptr) break;

		GetWorldKnowledgeWidget()->ChangeRoomProperties(Sensor->Name, Sensor->CheckIsRooomClean(), Sensor->CheckFornitureIsArranged(), Sensor->Opened);
	}
}

UWorldKnowledgeWidget* ARobotBTGameMode::GetWorldKnowledgeWidget() {
	// if already created, return it
	if (WorldKnowledgeWidgetInst != nullptr) return WorldKnowledgeWidgetInst;

	ARobotBTPlayerController* MyPlayerControllerInstance = Cast<ARobotBTPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (MyPlayerControllerInstance != nullptr) {
		WorldKnowledgeWidgetInst = MyPlayerControllerInstance->WorldKnowledgeWidget;
	}

	return WorldKnowledgeWidgetInst;
}

