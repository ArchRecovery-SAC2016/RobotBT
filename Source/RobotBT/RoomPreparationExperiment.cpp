// Copyright Epic Games, Inc. All Rights Reserved.

#include "RoomPreparationExperiment.h"
#include "Actors/Room.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ARoomPreparationExperiment::ARoomPreparationExperiment() {
	

}


void ARoomPreparationExperiment::BeginPlay() {
    Super::BeginPlay();


	// Load all Doors Sensors, so we can watch it
    TArray<AActor*> RoomsOnMap;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoom::StaticClass(), RoomsOnMap);

    for (AActor* Actor : RoomsOnMap) {
        ARoom* Room = Cast<ARoom>(Actor);
        if (Room != nullptr) {
        	Rooms.Add(Room);
		}
    }

	// Load cleaning robot
	TArray<AActor*> FoundRobots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobot::StaticClass(), FoundRobots);
	for (AActor* Actor : FoundRobots) {
		if (Actor->ActorHasTag("Cleaner")) {
			ARobotCleaner* Cleaner = Cast<ARobotCleaner>(Actor);
			if (Cleaner != nullptr) {
				CleanerRobot = Cleaner;
				CleanerRobot->OnTaskFinished.AddDynamic(this, &ARobotBTGameMode::CurrentTaskFinished);
			}
		}
	}

	// Load organization robot
	TArray<AActor*> FoundOrganizer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobot::StaticClass(), FoundOrganizer);
	for (AActor* Actor : FoundOrganizer) {
		if (Actor->ActorHasTag("Organizer")) {
			ARobotOrganizer* Organizer = Cast<ARobotOrganizer>(Actor);
			if (Organizer != nullptr) {
				Organizer->OnTaskFinished.AddDynamic(this, &ARobotBTGameMode::CurrentTaskFinished);
				OrganizersTeam.Add(Organizer);
			}
		}
	}

	CurrentTask = GetNextTask();

	ExecuteCurrentTask();
}

void ARoomPreparationExperiment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

	if (ExperimentIsOver) return;
}

void ARoomPreparationExperiment::ExecuteCurrentDecomposition() {
	const FTaskDecomposition& CurrentDecomposition = DecompositionQueue[CurrentDecompositionIndex];
	FString Arguments = CurrentDecomposition.Arguments;

	ARoom* RoomLocation = GetRoomByName("RoomA");
	// ARoom* RoomLocation = GetRoomByName(CurrentDecomposition.Arguments);
	if (CurrentDecomposition.Name == "sanitize-robot") {
		FString RobotName = "CleanerRobot";
		ExecuteSanitizeRobot(RobotName, RoomLocation);
	} else if (CurrentDecomposition.Name == "clean-room") {
		FString RobotName = "CleanerRobot";
		ExecuteClean(RobotName, RoomLocation);
	} else if (CurrentDecomposition.Name == "sanitize-robot") {
		FString RobotName = "CleanerRobot";
		ExecuteClean(RobotName, RoomLocation);
	} else if (CurrentDecomposition.Name == "move-furniture") {
		FString RobotName = "CleanerRobot";
		ExecuteClean(RobotName, RoomLocation);
	}

	UE_LOG(LogTemp, Log, TEXT("Executing Decomposition: %s, Arguments: %s"), *CurrentDecomposition.Name, *CurrentDecomposition.Arguments);
}

ARoom* ARoomPreparationExperiment::GetRoomByName(FString DoorName) {
	for (auto Door : Rooms) {
		if (Door->Name == DoorName) {
			return Door;
		}
	}

	return nullptr;
}

void ARoomPreparationExperiment::ExecuteClean(FString RobotName, ARoom* Room) {
	// sempre vai ser o cleaner robot, entao nao precisa passar o nome do robo
	CleanerRobot->StartCleaninTask(Room);
}

void ARoomPreparationExperiment::ExecuteMoveFurniture(FString RobotName, ARoom* Room) {
	return;
}

void ARoomPreparationExperiment::ExecuteOpenDoor(FString RobotName, ARoom* Room) {
	// sempre vai ser o cleaner robot, entao nao precisa procurar o robo
	CleanerRobot->StartSanitizationTask(Room);
}

void ARoomPreparationExperiment::ExecuteSanitizeRobot(FString RobotName, ARoom* Room) {
	// sempre vai ser o cleaner robot, entao nao precisa procurar o robo
	CleanerRobot->StartSanitizationTask(Room);
}