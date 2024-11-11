// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTGameMode.h"
#include "RobotBTPlayerController.h"
#include "Actors/Room.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Util/MyJsonReader.h"
#include "Util/UtilMethods.h"

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
	LoadTasksFromFile();

	// Load all Doors Sensors, so we can watch it
    TArray<AActor*> RoomsOnMap;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoom::StaticClass(), RoomsOnMap);

    for (AActor* Actor : RoomsOnMap) {
        ARoom* Sensor = Cast<ARoom>(Actor);
        if (Sensor != nullptr) {
        	Rooms.Add(Sensor);
		}
    }

	// Load cleaning robot
	TArray<AActor*> FoundRobots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotCleaner::StaticClass(), FoundRobots);
	for (AActor* Actor : FoundRobots) {
		ARobotCleaner* Cleaner = Cast<ARobotCleaner>(Actor);
		if (Cleaner != nullptr) {
			CleanerRobot = Cleaner;
			Cleaner->OnRoomCleaned.AddDynamic(this, &ARobotBTGameMode::OnRoomCleaned);
			Cleaner->OnDoorOpened.AddDynamic(this, &ARobotBTGameMode::OnDoorOpened);
			// Cleaner->OnRobotSanitized.AddDynamic(this, &ARobotBTGameMode::OnRobotSanitized);
		}
	}

	// Load organization robot
	TArray<AActor*> FoundOrganizer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotOrganizer::StaticClass(), FoundOrganizer);
	for (AActor* Actor : FoundOrganizer) {
		ARobotOrganizer* Organizer = Cast<ARobotOrganizer>(Actor);
		if (Organizer != nullptr) {
			Organizer->OnFurnitureMoveEnded.AddDynamic(this, &ARobotBTGameMode::OnFurnitureMoveEnded);
			OrganizersTeam.Add(Organizer);
		}
	}

	CurrentTask = GetNextTask();

	ExecuteCurrentTask();
}

void ARobotBTGameMode::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

	UpdateWorldKnowledgeWidget();

	if (ExperimentIsOver) return;

}

void ARobotBTGameMode::UpdateWorldKnowledgeWidget() {
	for (ARoom* Sensor: Rooms) {
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

ARoom* ARobotBTGameMode::GetTaskRoom() {
	if (CurrentTask == nullptr) return nullptr;
	
	for (auto Door :  Rooms) {
		if (Door->Name == CurrentTask->Locations) {
			return Door;
		}
	}

	return nullptr;
}

void ARobotBTGameMode::LoadTasksFromFile() {
	Tasks = UMyJsonReader::ReadJsonFile();
}

FTask* ARobotBTGameMode::GetNextTask() {
	 // Check if the tasks map is not empty
	 if (Tasks.Num() == 0 || ExperimentIsOver) {
		 return nullptr;
	 }

	 // Retrieve all keys of the map
	 TArray<FString> Keys;
	 Tasks.GetKeys(Keys);

	 while (CurrentTaskIndex < Tasks.Num()) {
		FTask* Task = Tasks.Find(Keys[CurrentTaskIndex]);

		if (Task != nullptr) {
			// FString TaskMessage = FString::Printf(TEXT("Starting New Task: %s. With Name: %s "), *Task->Id, *Task->Name);
			// UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::INFO);

			if (CheckPreCondition(Task)) {
				CurrentTaskIndex++;
				return Task;
			} else {
				// if fails, we try another
				CurrentTaskIndex++;
			}
		} else {
			UUtilMethods::ShowLogMessage(TEXT("No task found! Experiment is over"), EMessageColorEnum::ERROR);
			ExperimentIsOver = true;
		}
	}

	 UUtilMethods::ShowLogMessage(TEXT("No task found! Experiment is over"), EMessageColorEnum::ERROR);
	 return nullptr;
}


void ARobotBTGameMode::ExecuteCurrentTask() {
	if (CurrentTask == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTaskIterator is null!"));
		return;
	}

	if (CurrentTask != nullptr && CurrentTask->Decomposition.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTask.Decomposition is empty!"));
		return;
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

	} else {
		// reset the decomposition array
		DecompositionQueue.Empty();

		// reset the decomposition index
		CurrentDecompositionIndex = 0;

		// If gets here, all decomposition was executed, and we can got to the next task
		CurrentTask = GetNextTask();

		if (CurrentTask == nullptr){
			UUtilMethods::ShowLogMessage(TEXT("No more task, Experiment is over"), EMessageColorEnum::SUCCESS);
			return;
		}

		// and try to execut it 
		ExecuteCurrentTask();
	}
}

bool ARobotBTGameMode::CheckPreCondition(FTask* NewTask) {
	bool ReturnValue = true;

	if (NewTask == nullptr){
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTaskIterator is null!"));
		return ReturnValue;
	}

	for (int32 i=0; i < NewTask->Preconditions.Num(); i++) {
		FTaskPrecondition CurrentPrecondition = NewTask->Preconditions[i];

		// in the example: "predicate": "RoomA.door_open",
		FString ObjectName; // Will be RoomA
		FString Condition; // Will be door_open
		if (!ParsePredicate(CurrentPrecondition.Predicate, ObjectName, Condition)) {
			UE_LOG(LogTemp, Error, TEXT("Failed to parse predicate: %s"), *CurrentPrecondition.Predicate);
			return false;
		}

		if (CurrentPrecondition.VarTypes == TEXT("room")) {
			auto Door = GetDoorByName(CurrentPrecondition.Vars);
			bool Negation = ObjectName.StartsWith(TEXT("not ")); // some predicates starts with not, so we need to check it

			if (Door == nullptr) {
				// FString TaskMessage = FString::Printf(TEXT("PreCondition Failed: Door not found:: %s!"), *CurrentPrecondition.Vars);
				// UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::ERROR);
				return false;
			}

			if (Condition == TEXT("door_open")) {
				if (Negation) { // if is a negation, line not RoomA.door_open, we need to check if the door is closed
					if (Door->Opened == true) {
						FString TaskMessage = FString::Printf(TEXT("PreCondition Failed: Door is already open: %s!"), *CurrentPrecondition.Predicate);
						UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::ERROR);
						return false;
					}
				} else {
					if(Door->Opened == false) {
						FString TaskMessage = FString::Printf(TEXT("PreCondition Failed: Door is not open: %s!"), *CurrentPrecondition.Predicate);
						UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::ERROR);
						return false;
					}
				}
			} else if (Condition == TEXT("is_clean")) {
				if (Negation) {
					if (Door->CheckIsRooomClean() == true) {
						FString TaskMessage = FString::Printf(TEXT("PreCondition Failed: Room already is clear: %s!"), *CurrentPrecondition.Predicate);
						UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::ERROR);
						return false;
					}
				} else {
					if (Door->CheckIsRooomClean() == false) {
						FString TaskMessage = FString::Printf(TEXT("PreCondition Failed: Room not is clear: %s!"), *CurrentPrecondition.Predicate);
						UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::ERROR);
						return false;
					}
				}
			} else if (Condition == TEXT("sanitize-robot")) {
				if (Door->Opened) {
					FString TaskMessage = FString::Printf(TEXT("PreCondition Failed: Door is not open: %s!"), *CurrentPrecondition.Predicate);
					UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::ERROR);
					return false;
				}
			}
		} else if (CurrentPrecondition.VarTypes == TEXT("robot")) {
			if (CurrentPrecondition.Predicate == TEXT("not ?r.is_sanitized")) {
				/*
				if (CleanerRobot->IsRobotSanitized) {
					FString TaskMessage = FString::Printf(TEXT("PreCondition Failed: Robot is already sanitized: %s!"), *CurrentPrecondition.Predicate);
					UUtilMethods::ShowLogMessage(TaskMessage, EMessageColorEnum::ERROR);
					return false;
				} else {
					return true;
				}
				*/
			}
			
			UUtilMethods::ShowLogMessage(TEXT("Precondition of type robot Not Found, returning true"), EMessageColorEnum::ERROR);
			return true;

		} else {
			UUtilMethods::ShowLogMessage(TEXT("Precondition Not Found, returning true"), EMessageColorEnum::ERROR);
			return true;
		}
	}


	return ReturnValue;
}


bool ARobotBTGameMode::ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition) {
	return Predicate.Split(TEXT("."), &OutObjectName, &OutCondition);
}

ARoom* ARobotBTGameMode::GetDoorByName(const FString& DoorName) {
	for (auto Door : Rooms) {
		if (Door->Name == DoorName) {
			return Door;
		}
	}

	return nullptr;
}

void ARobotBTGameMode::OnRoomCleaned(bool bNewState) {
	if (bNewState ) {
		UUtilMethods::ShowLogMessage(TEXT("Room Cleaned!"), EMessageColorEnum::SUCCESS);
	} else {
		UUtilMethods::ShowLogMessage(TEXT("Room not Cleaned!"), EMessageColorEnum::ERROR);
	}

	CurrentDecompositionIndex++;
	// Continue executing tasks 
	ExecuteCurrentTask();

}

void ARobotBTGameMode::OnDoorOpened(bool bNewState) {
	if (bNewState) {
		UUtilMethods::ShowLogMessage(TEXT("Door Opened!"), EMessageColorEnum::SUCCESS);
	} else {
		UUtilMethods::ShowLogMessage(TEXT("Couldn't open Door"), EMessageColorEnum::ERROR);
	}

	CurrentDecompositionIndex++;
	// Continue executing tasks 
	ExecuteCurrentTask();

}

void ARobotBTGameMode::OnRobotSanitized(bool bNewState) {
	if (bNewState) {
		UUtilMethods::ShowLogMessage(TEXT("Robot Sanitized!"), EMessageColorEnum::SUCCESS);
	} else {
		UUtilMethods::ShowLogMessage(TEXT("Robot couldn'Sanitized"), EMessageColorEnum::ERROR);
	}

	CurrentDecompositionIndex++;
	// Continue executing tasks 
	ExecuteCurrentTask();
}

void ARobotBTGameMode::OnFurnitureMoveEnded(bool bNewState) {
	if (bNewState) {
		UUtilMethods::ShowLogMessage(TEXT("Robot Sanitized!"), EMessageColorEnum::SUCCESS);
	}
	else {
		UUtilMethods::ShowLogMessage(TEXT("Robot couldn'Sanitized"), EMessageColorEnum::ERROR);
	}

	bool AllFinished = true;
	for (auto Organizer : OrganizersTeam) {
		if (Organizer->IsRobotMovingFurniture()) {
			AllFinished = false;
			break;
		}
	}

	// if all finished, we continue executing tasks 
	if (AllFinished) {
		CurrentDecompositionIndex++;
	
		ExecuteCurrentTask();
	}
}

