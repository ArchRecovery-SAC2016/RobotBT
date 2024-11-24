#include "RoomPreparationExperiment.h"
#include "Actors/Room.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Util/MyCSVReader.h"
#include "Util/MyJsonReader.h"
#include "Util/UtilMethods.h"

ARoomPreparationExperiment::ARoomPreparationExperiment() {
	

}

void ARoomPreparationExperiment::BeginPlay() {
    Super::BeginPlay();

	// altera vecolidade pra ficar 10x mais rapido
	if (GetWorld()) {
		GetWorld()->GetWorldSettings()->SetTimeDilation(10.0f);
	}

	// Load all Doors Sensors, so we can watch it
    TArray<AActor*> RoomsOnMap;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomPreparation::StaticClass(), RoomsOnMap);

    for (AActor* Actor : RoomsOnMap) {
        ARoomPreparation* Room = Cast<ARoomPreparation>(Actor);
        if (Room != nullptr) {
        	Rooms.Add(Room);
		}
    }

	// Load cleaning robot
	TArray<AActor*> FoundRobots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotCleaner::StaticClass(), FoundRobots);
	for (AActor* Actor : FoundRobots) {
		ARobotCleaner* Cleaner = Cast<ARobotCleaner>(Actor);
		if (Cleaner != nullptr) {
			CleanerRobot = Cleaner;
			CleanerRobot->OnTaskFinished.AddDynamic(this, &ABaseExperiment::CurrentTaskFinished);
		}
	}

	// Load organization robot
	TArray<AActor*> FoundOrganizer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotOrganizer::StaticClass(), FoundOrganizer);
	for (AActor* Actor : FoundOrganizer) {
		ARobotOrganizer* Organizer = Cast<ARobotOrganizer>(Actor);
		if (Organizer != nullptr) {
			Organizer->OnTaskFinished.AddDynamic(this, &ABaseExperiment::CurrentTaskFinished);
			OrganizersTeam.Add(Organizer);
		}
	}

	if (CleanerRobot == nullptr ) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Cleaner Robot. Please add at least one. No task will be executed!"));
		return;
	}

	if (OrganizersTeam.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Organizer Team. Please add at least one. No task will be executed!"));
		return;
	}

	// StartDefaultExperiment();
	GenerateRandomProperties = false;
	StartExperiment(1);
}

void ARoomPreparationExperiment::StartExperiment(int32 numberOfTimes) {
	UMyCSVReader::CreateCSVFile(false);
	LoadTasksFromFile("RoomPreparation", 1);	// load all tasks from file

	if (!LoadWorldFromFile("RoomPreparation", 1)) return;

	PrepareWorld();

	ExperimentId = 0; // start with id 0. ExecuteNextExperiment will increment
	RepeatExperiment = numberOfTimes;
	ExperimentIsOver = true; // this will make enter tick loop and executethe next 
}

void ARoomPreparationExperiment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

	if (ExperimentIsOver == true && ExperimentId < RepeatExperiment) {
		ExecuteNextExperiment();
	}
}

void ARoomPreparationExperiment::ExecuteNextExperiment() {
	FString Message = FString::Printf(TEXT("Executing Experiment With Id: %d"), Experiment.ExperimentId);
	UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);
	ExperimentIsOver = false; // this willa void enter tick loop

	ExperimentId++;
	Experiment.ExperimentId = ExperimentId;
	Experiment.Approach = "Baseline";
	Experiment.ExperimentTime = 0;
	CurrentTaskIndex = -1;
	CurrentTask = GetNextTask();
	ExecuteCurrentTask();

	if (GenerateRandomProperties) {
		CleanerRobot->GenerateRandomProperties();
		for (auto* Organizer : OrganizersTeam) {
			Organizer->GenerateRandomProperties();
		}
	}
}

bool ARoomPreparationExperiment::CheckPreCondition(FTask* NewTask) {
	if (NewTask == nullptr) return false;

	for (const FPredicate& Predicate : NewTask->Preconditions) {
		if (!EvaluatePreCondition(Predicate)) {
			
			FString ConditionMessage = FString::Printf(TEXT("%s %s %s"), *Predicate.Variable, Predicate.bNegated ? TEXT("not ") : TEXT(""), *Predicate.Condition);
			FString Message = FString::Printf(TEXT("Precondition failed: %s Variable: %s"), *ConditionMessage, *Predicate.Variable);
			UE_LOG(LogTemp, Error, TEXT("%s"), *Message);

			return false;
		}
	}
	return true; // Todas as precondições foram satisfeitas
}

bool ARoomPreparationExperiment::EvaluatePreCondition(const FPredicate& Predicate) {
	if (Predicate.Variable.Contains("Room")) {
		// Trata-se de uma sala
		ARoomPreparation* Room = GetRoomByName(Predicate.Variable);
		if (!Room) {
			UE_LOG(LogTemp, Error, TEXT("Room not found: %s"), *Predicate.Variable);
			return false;
		}

		if (Predicate.Condition == "door_open") {
			if (Predicate.bNegated) return !Room->DoorOpened; // Negated
			return Room->DoorOpened;
		}
		if (Predicate.Condition == "is_clean") {
			if (Predicate.bNegated) return !Room->IsTrashClean(); // Negated
			return Room->IsTrashClean(); 
		}
	}
	else if (Predicate.Variable.Contains("?r")) {
		if (Predicate.Condition == "is_sanitized") {
			if (Predicate.bNegated) return !CleanerRobot->IsSanitized; // Negated
			return CleanerRobot->IsSanitized;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Unhandled Predicate: %s"), *Predicate.Condition);
	return false;
}


FTask* ARoomPreparationExperiment::GetNextTask() {
	FTask* NewTask = Super::GetNextTask();
	if (NewTask == nullptr) {
		ExperimentIsOver = true;
		return nullptr;
	}

	FString Message = FString::Printf(TEXT("Next Task Id: %s name: %s"), *NewTask->Id, *NewTask->Name);
	UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);
	
	if (CheckPreCondition(NewTask)) {
		return NewTask;
	}
	// if fails, we try another calling recursively. The super will stop when the task is null
	return GetNextTask();
}

void ARoomPreparationExperiment::PrepareWorld() {
	for (FWorldRoomDataStruct RoomData : WorldRoomsStruct) {
		ARoomPreparation* Room = GetRoomByName(RoomData.Name);
		if (Room == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("Room not found!"));
			return;
		}
		Room->Initiate(RoomData);
	}
}

void ARoomPreparationExperiment::ExecuteCurrentDecomposition() {
	// A decomposicao esta bem confusa. Ela esta com os argumentos incompletos. Entao vamos usar apenas o nome e pegamos o local das tasks. 
	const FTaskDecomposition& CurrentDecomposition = DecompositionQueue[CurrentDecompositionIndex];
	FString RoomName = CurrentTask->Locations;

	ARoomPreparation* RoomLocation = GetRoomByName(RoomName);
	if (RoomLocation == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Room not found!"));
		return;
	}

	if (CurrentDecomposition.Name == "sanitize-robot") {
		FString RobotName = "CleanerRobot";
		ExecuteSanitizeRobot(RobotName, RoomLocation);
	} else if (CurrentDecomposition.Name == "clean-room") {
		FString RobotName = "CleanerRobot";
		ExecuteClean(RobotName, RoomLocation);
	} else if (CurrentDecomposition.Name == "open-door") {
		FString RobotName = "CleanerRobot";
		ExecuteOpenDoor(RobotName, RoomLocation);
	} else if (CurrentDecomposition.Name == "move-furniture") {
		FString RobotName = "CleanerRobot";
		ExecuteMoveFurniture(RobotName, RoomLocation);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Executing Decomposition: %s, Arguments: %s"), *CurrentDecomposition.Name, *CurrentDecomposition.Arguments);
}

ARoomPreparation* ARoomPreparationExperiment::GetRoomByName(FString DoorName) {
	for (auto Room : Rooms) {
		if (Room->Name == DoorName) {
			return Room;
		}
	}

	return nullptr;
}

void ARoomPreparationExperiment::ExecuteClean(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->ExecuteTask(ESkillEnum::CLEAN_ROOM, Room);
}

void ARoomPreparationExperiment::ExecuteOpenDoor(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->ExecuteTask(ESkillEnum::OPEN_DOOR, Room);
}

void ARoomPreparationExperiment::ExecuteSanitizeRobot(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->ExecuteTask(ESkillEnum::SANITIZE_ROBOT, Room);
}

void ARoomPreparationExperiment::ExecuteMoveFurniture(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 2;
	for (ARobotOrganizer* Organizer : OrganizersTeam) {
		Organizer->ExecuteTask(ESkillEnum::MOVE_FURNITURE, Room);
	};
}