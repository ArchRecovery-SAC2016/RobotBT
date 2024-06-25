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

	CurrentTask = GetNextTask();

}

void ARobotBTGameMode::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    UpdateWorldKnowledgeWidget();
	bool TaskEnd = ExecuteCurrentTask();

	if (TaskEnd) {
		CurrentTask = GetNextTask();
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

ADoorSensor* ARobotBTGameMode::GetNextRoomToBePrepared() {
	if (CurrentTask == nullptr) return nullptr;
	
	for (auto Door :  DoorSensors) {
		if (Door->Name == CurrentTask->Locations) {
			return Door;
		}
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

FTask* ARobotBTGameMode::GetNextTask() {
	// reset the decomposition array
	DecompositionQueue.Empty();

	// reset the decomposition index
	 CurrentDecompositionIndex = 0;

	 // Check if the tasks map is not empty
	 if (Tasks.Num() == 0) {
		 return nullptr;
	 }

	 // If it's the first time or CurrentTaskIndex is out of bounds, reset the index
	 if (CurrentTaskIndex < 0 || CurrentTaskIndex >= Tasks.Num()) {
		 CurrentTaskIndex = 0;
	 }

	 // Retrieve all keys of the map
	 TArray<FString> Keys;
	 Tasks.GetKeys(Keys);

	 // Get the task by the current index
	 FTask* Task = Tasks.Find(Keys[CurrentTaskIndex]);

	 // Increment the index for the next call
	 CurrentTaskIndex++;

	 // If CurrentTaskIndex goes out of bounds, reset it
	 if (CurrentTaskIndex >= Tasks.Num()) {
		 CurrentTaskIndex = 0;
	 }

	 return Task;
	
}

bool ARobotBTGameMode::ExecuteCurrentTask() {
	if (CurrentTask == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTaskIterator is null!"));
		return false;
	}

	if (CurrentTask->Decomposition.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTask.Decomposition is empty!"));
		return false;
	}

	// if is empty, so is the first time of this task, so we fill the decomposition queue
	if (DecompositionQueue.IsEmpty())	{
		for (auto it = CurrentTask->Decomposition.CreateConstIterator(); it; ++it) {
			DecompositionQueue.Add(it.Value());
		}
	}

	if (CurrentDecompositionIndex < DecompositionQueue.Num()) {
		const FTaskDecomposition& CurrentDecomposition = DecompositionQueue[CurrentDecompositionIndex];

		UE_LOG(LogTemp, Log, TEXT("Executing Decomposition: %s, Arguments: %s"), *CurrentDecomposition.Name, *CurrentDecomposition.Arguments);

		if (CurrentDecomposition.Name == TEXT ("clean-room")) {
			ShowLogMessage(TEXT("Executing Cleaning Room Task!"));
			if (Cleaning_Tick()) {  // will return true if the cleaning tick is completed
				CurrentDecompositionIndex++;
			} 
		} else if (CurrentDecomposition.Name == TEXT ("open-door")) {
			ShowLogMessage(TEXT("Executing Open Door Task!"));

			UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] Open Door is not implemented!"));
			return false;
		} else if (CurrentDecomposition.Name == TEXT ("sanitize-robot")) {
			ShowLogMessage(TEXT("Executing SanitizeRobot Task!"));
			UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] SanitizeRobot is not implemented!"));
			return false;
		} else if (CurrentDecomposition.Name == TEXT("move-furniture")) {
			ShowLogMessage(TEXT("Executing MoveFurniture Task!"));
			UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] MoveFurniture is not implemented!"));
			return false;
		}

	} else {


		// If gets here, all decomposition was executed
		return true;
	}

	return false;
}

void ARobotBTGameMode::ShowLogMessage(const FString& Message) {
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Message);
	}
}
