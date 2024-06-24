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

	// Load cleaning robot
	TArray<AActor*> FoundRobots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotCleaner::StaticClass(), FoundRobots);
	for (AActor* Actor : FoundRobots) {
		ARobotCleaner* Cleaner = Cast<ARobotCleaner>(Actor);
		if (Cleaner != nullptr) {
			CleanerRobot = Cleaner;
		}
	}


	// Load organization robot
	TArray<AActor*> FoundOrganizer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotOrganizer::StaticClass(), FoundOrganizer);
	for (AActor* Actor : FoundOrganizer) {
		ARobotOrganizer* Organizer = Cast<ARobotOrganizer>(Actor);
		if (Organizer != nullptr) {
			OrganizersTeam.Add(Organizer);
		}
	}

}

void ARobotBTGameMode::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

   UpdateWorldKnowledgeWidget();

	if (ActiveAction == EActionsEnum::CLEAN_ROOM) {
		Cleaning_Tick();
		
	}
}

void ARobotBTGameMode::UpdateWorldKnowledgeWidget() {
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

bool ARobotBTGameMode::Cleaning_Tick() {
	if (RoomSelected == nullptr) {
		RoomSelected = GetNextRoomToBePrepared();
	}


	if (CleanerRobot->CleanRoom(RoomSelected))
	{
		RoomSelected = nullptr;

		return true;
	}

	return false;

}

void ARobotBTGameMode::FindNewAction() {
	

}

TArray<ADoorSensor*> ARobotBTGameMode::GetRoomsToBePrepared() {
	TArray<ADoorSensor*> PreparedRooms;

	for (ADoorSensor* Door : DoorSensors) {
		if (Door->IsPrepared() == false) {
			PreparedRooms.Add(Door);
		}
	}

	return PreparedRooms;
}

ADoorSensor* ARobotBTGameMode::GetNextRoom() {

	return nullptr;
}

ADoorSensor* ARobotBTGameMode::GetNextRoomToBePrepared() {
	TArray<ADoorSensor*> PreparedRooms = GetRoomsToBePrepared();

	if (PreparedRooms.Num() > 0) {
		return PreparedRooms[0];
	}

	return nullptr;
}

bool ARobotBTGameMode::CheckPreconditions() {
		return false;
}

bool ARobotBTGameMode::CheckEffects() {
		return false;
}
