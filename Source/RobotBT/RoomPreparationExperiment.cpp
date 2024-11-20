#include "RoomPreparationExperiment.h"
#include "Actors/Room.h"
#include "Enum/MessageColorEnum.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
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
			CleanerRobot->OnTaskFinished.AddDynamic(this, &ARobotBTGameMode::CurrentTaskFinished);
		}
	}

	// Load organization robot
	TArray<AActor*> FoundOrganizer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotOrganizer::StaticClass(), FoundOrganizer);
	for (AActor* Actor : FoundOrganizer) {
		if (Actor->ActorHasTag("Organizer")) {
			ARobotOrganizer* Organizer = Cast<ARobotOrganizer>(Actor);
			if (Organizer != nullptr) {
				Organizer->OnTaskFinished.AddDynamic(this, &ARobotBTGameMode::CurrentTaskFinished);
				OrganizersTeam.Add(Organizer);
			}
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

	CurrentTask = GetNextTask();

	ExecuteCurrentTask();
}

void ARoomPreparationExperiment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

	if (ExperimentIsOver) return;
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
	UUtilMethods::ShowLogMessage(TEXT("Initiating task clean-room "), EMessageColorEnum::INFO);
	CleanerRobot->StartCleaninTask(Room);
}

void ARoomPreparationExperiment::ExecuteMoveFurniture(FString RobotName, ARoomPreparation* Room) {
	UUtilMethods::ShowLogMessage(TEXT("Initiating task move-furniture"), EMessageColorEnum::INFO);

	for (ARobotOrganizer* organizer: OrganizersTeam) {
		organizer->StartOrganizeTask(Room);
	}
}

void ARoomPreparationExperiment::ExecuteOpenDoor(FString RobotName, ARoomPreparation* Room) {
	UUtilMethods::ShowLogMessage(TEXT("Initiating task open-door"), EMessageColorEnum::INFO);
	CleanerRobot->StartOpenDoorTask(Room);
}

void ARoomPreparationExperiment::ExecuteSanitizeRobot(FString RobotName, ARoomPreparation* Room) {
	UUtilMethods::ShowLogMessage(TEXT("Initiating task sanitize-robot "), EMessageColorEnum::INFO);
	CleanerRobot->StartSanitizationTask(Room);
}