// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTGameMode.h"
#include "RobotBTPlayerController.h"
#include "RobotBTCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Util/MyJsonReader.h"

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

	// load all tasks from file
	LoadTasks();

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
	bool TaskEnd = ExecuteCurrentTask();

	if (TaskEnd) {
		GetNextTask();
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

	if (CleanerRobot->CleanRoom(RoomSelected)) {
		RoomSelected = nullptr;

		return true;
	}

	return false;

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

void ARobotBTGameMode::LoadTasks() {
	Tasks = UMyJsonReader::ReadJsonFile();
}

TMap<FString, FTask>::TConstIterator* ARobotBTGameMode::GetNextTask() {
	// if the first time
	if (CurrentTaskIterator == nullptr) {
		CurrentTaskIterator = new TMap<FString, FTask>::TConstIterator(Tasks.CreateConstIterator());
		return CurrentTaskIterator;
	}

	if (CurrentTaskIterator && Tasks.Num() > 0) {
		++CurrentTaskIterator;
		if (CurrentTaskIterator) {
			return CurrentTaskIterator;
		}
	}


	return nullptr;
}

bool ARobotBTGameMode::ExecuteCurrentTask() {
	if (CurrentTaskIterator == nullptr) return false;

	const FTask& CurrentTask = CurrentTaskIterator->Value();

	if (CurrentDecompositionIndex < DecompositionQueue.Num()) {
		const FTaskDecomposition& CurrentDecomposition = DecompositionQueue[CurrentDecompositionIndex];

		UE_LOG(LogTemp, Log, TEXT("Executing Decomposition: %s, Arguments: %s"), *CurrentDecomposition.Name, *CurrentDecomposition.Arguments);

		if (CurrentDecomposition.Name == TEXT ("clean-room")) {
			
			if (Cleaning_Tick()) {  // will return true if the cleaning tick is completed
				CurrentDecompositionIndex++;
			}
		}

	}else {

		// If gets here, all decomposition was executed
		return true;
	}

	return false;
}
