#include "ExperimentRoomPreparation.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "RobotBT/Actors/Room.h"
#include "RobotBT/Util/MyJsonReader.h"

AExperimentRoomPreparation::AExperimentRoomPreparation() {

}

void AExperimentRoomPreparation::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AExperimentRoomPreparation::BeginPlay() {
    Super::BeginPlay();

	// G2
	FetchRoomsToBePrepared();
    

	// Load cleaning robot
	TArray<AActor*> FoundRobots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotCleaner::StaticClass(), FoundRobots);
	for (AActor* Actor : FoundRobots) {
		ARobotCleaner* Cleaner = Cast<ARobotCleaner>(Actor);
		if (Cleaner != nullptr) {
			CleanerRobot = Cleaner;
			CleanerRobot->OnTaskFinished.AddDynamic(this, &AExperiment::CurrentTaskFinished);
		}
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
	}

	if (CleanerRobot == nullptr ) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Cleaner Robot. Please add at least one. No task will be executed!"));
		return;
	}

	if (OrganizersTeam.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Organizer Team. Please add at least one. No task will be executed!"));
		return;
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
	return true; // Todas as precondicoees foram satisfeitas
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
	UE_LOG(LogTemp, Display, TEXT("Preparing the world for Experiment: %d"), CurrentExperiment.ExperimentId);

	// prepare the rooms
	for (FWorldRoomDataStruct RoomData : WorldRoomsStruct) {
		ARoomPreparation* Room = GetRoomByName(RoomData.Name);
		if (Room == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("Room not found!"));
			return;
		}
		Room->Initiate(RoomData);
	}

	// clean the array of robots properties
	RobotsProperties.Empty();

	// initiate the robots
	CleanerRobot->Initiate(CurrentExperiment.GenerateRandomProperties);
	RobotsProperties.Add(CleanerRobot->RobotProperties);
	for (auto* Organizer : OrganizersTeam) {
		Organizer->Initiate(CurrentExperiment.GenerateRandomProperties);
		RobotsProperties.Add(Organizer->RobotProperties);
	}
}

void AExperimentRoomPreparation::TimeIsOver() {
	Super::TimeIsOver();

	CleanerRobot->TimeIsOver();
	for (auto* Organizer : OrganizersTeam) {
		Organizer->TimeIsOver();
	}
}

void AExperimentRoomPreparation::FetchRoomsToBePrepared() {
	Super::FetchRoomsToBePrepared();

	// Load all Doors Sensors, so we can watch it
	TArray<AActor*> RoomsOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomPreparation::StaticClass(), RoomsOnMap);

	for (AActor* Actor : RoomsOnMap) {
		ARoomPreparation* Room = Cast<ARoomPreparation>(Actor);
		if (Room != nullptr && !Room->IsRoomPrepared()) {
			Rooms.Add(Room);
		}
	}

	UGoalTracker::Evaluate_G2(GoalModel, Rooms);
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
		FString RobotName = "OrganizerTeam";
		ExecuteMoveFurniture(RobotName, RoomLocation);
	}
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
	CleanerRobot->StartNewTask(ESkillEnum::CLEAN_ROOM, Room);
}

void AExperimentRoomPreparation::ExecuteOpenDoor(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->StartNewTask(ESkillEnum::OPEN_DOOR, Room);
}

void AExperimentRoomPreparation::ExecuteSanitizeRobot(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 1;
	CleanerRobot->StartNewTask(ESkillEnum::SANITIZE_ROBOT, Room);
}

void AExperimentRoomPreparation::ExecuteMoveFurniture(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 2;
	for (ARobotOrganizer* Organizer : OrganizersTeam) {
		Organizer->StartNewTask(ESkillEnum::MOVE_FURNITURE, Room);
	};
}