#include "ExperimentRoomPreparation.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "RobotBT/Util/MyCSVReader.h"
#include "RobotBT/Actors/Room.h"
#include "RobotBT/Util/MyJsonReader.h"

AExperimentRoomPreparation::AExperimentRoomPreparation() {

}

void AExperimentRoomPreparation::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AExperimentRoomPreparation::BeginPlay() {
    Super::BeginPlay();

	// altera vecolidade pra ficar 10x mais rapido
	if (GetWorld()) {
		GetWorld()->GetWorldSettings()->SetTimeDilation(ExperimentSpeed);
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
			CleanerRobot->OnTaskFinished.AddDynamic(this, &AExperiment::CurrentTaskFinished);
		}
		// saves the initial transform
		CleanerInitialTransform = CleanerRobot->GetActorTransform();
	}

	// Load organization robot
	TArray<AActor*> FoundOrganizer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotOrganizer::StaticClass(), FoundOrganizer);
	for (AActor* Actor : FoundOrganizer) {
		ARobotOrganizer* Organizer = Cast<ARobotOrganizer>(Actor);
		if (Organizer != nullptr) {
			Organizer->OnTaskFinished.AddDynamic(this, &AExperiment::CurrentTaskFinished);
			OrganizersTeam.Add(Organizer);
		}

		OrganizeInitialTransform.Add(Organizer->GetActorTransform());
	}

	if (CleanerRobot == nullptr ) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Cleaner Robot. Please add at least one. No task will be executed!"));
		return;
	}

	if (OrganizersTeam.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Organizer Team. Please add at least one. No task will be executed!"));
		return;
	}

	// Start the experiment based on the properties set on the GM_ExperimentRoomPreparation	Blueprint. (in the editor)
	StartExperiment();
}

void AExperimentRoomPreparation::StartExperiment() {
	LoadTasksFromFile();	// load all tasks from file

	if (!LoadWorldFromFile()) return;

	PrepareWorld();

	ExperimentId = -1; // start with id 0. ExecuteNextExperiment will increment

	ExecuteNextExperiment();
}

void AExperimentRoomPreparation::SetRandomRobotsProperties() {
	// Generate Random Properties for the Cleaner
	CleanerRobot->GenerateRandomProperties();

	// Generate Random Properties for the Organizers
	for (auto* Organizer : OrganizersTeam) {
		Organizer->GenerateRandomProperties();
	}
}

bool AExperimentRoomPreparation::CheckPreCondition(FTask* NewTask) {
	if (NewTask == nullptr) return false;

	for (const FPredicate& Predicate : NewTask->Preconditions) {
		if (!EvaluatePreCondition(Predicate)) {
			
			FString ConditionMessage = FString::Printf(TEXT("%s %s %s"), *Predicate.Variable, Predicate.bNegated ? TEXT("not ") : TEXT(""), *Predicate.Condition);
			FString Message = FString::Printf(TEXT("Precondition failed: %s Variable: %s"), *ConditionMessage, *Predicate.Variable);
			UE_LOG(LogTemp, Error, TEXT("%s"), *Message);

			return false;
		}
	}
	return true; // Todas as precondi��es foram satisfeitas
}

bool AExperimentRoomPreparation::EvaluatePreCondition(const FPredicate& Predicate) {
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
			if (Predicate.bNegated) return !CleanerRobot->IsSanitized(); // Negated
			return CleanerRobot->IsSanitized();
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Unhandled Predicate: %s"), *Predicate.Condition);
	return false;
}

void AExperimentRoomPreparation::PrepareWorld() {
	// prepare the rooms
	for (FWorldRoomDataStruct RoomData : WorldRoomsStruct) {
		ARoomPreparation* Room = GetRoomByName(RoomData.Name);
		if (Room == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("Room not found!"));
			return;
		}
		Room->Initiate(RoomData);
	}

	// prepare the robots, to stay in the intial position
	CleanerRobot->SetActorTransform(CleanerInitialTransform);
	for (auto* Organizer : OrganizersTeam) {
		Organizer->SetActorTransform(OrganizeInitialTransform[OrganizersTeam.IndexOfByKey(Organizer)]);
	}
}

void AExperimentRoomPreparation::ExecuteCurrentDecomposition() {
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

ARoomPreparation* AExperimentRoomPreparation::GetRoomByName(FString DoorName) {
	for (auto Room : Rooms) {
		if (Room->Name == DoorName) {
			return Room;
		}
	}

	return nullptr;
}

void AExperimentRoomPreparation::ExecuteClean(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->ExecuteTask(ESkillEnum::CLEAN_ROOM, Room);
}

void AExperimentRoomPreparation::ExecuteOpenDoor(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->ExecuteTask(ESkillEnum::OPEN_DOOR, Room);
}

void AExperimentRoomPreparation::ExecuteSanitizeRobot(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->ExecuteTask(ESkillEnum::SANITIZE_ROBOT, Room);
}

void AExperimentRoomPreparation::ExecuteMoveFurniture(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 2;
	for (ARobotOrganizer* Organizer : OrganizersTeam) {
		Organizer->ExecuteTask(ESkillEnum::MOVE_FURNITURE, Room);
	};
}