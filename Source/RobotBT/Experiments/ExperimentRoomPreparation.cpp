#include "ExperimentRoomPreparation.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "RobotBT/Actors/Room.h"
#include "RobotBT/Struct/ValidationStruct.h"
#include "RobotBT/Util/MyJsonReader.h"

AExperimentRoomPreparation::AExperimentRoomPreparation() {

}

void AExperimentRoomPreparation::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AExperimentRoomPreparation::BeginPlay() {
    Super::BeginPlay();

	// Load all Rooms
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

void AExperimentRoomPreparation::ExecuteExperiment(FExperimentResult& NewExperiment) {
	// G2 Rooms to be Prepared Are Fetched
	FetchRoomsToBePrepared();

	Super::ExecuteExperiment(NewExperiment);
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
			bool RoomIsClean = Room->IsTrashClean();
			UE_LOG(LogTemp, Error, TEXT("Check if Room: %s is Clean. Result: %s"), *Room->Name, RoomIsClean ? TEXT("true") : TEXT("false"));

			if (Predicate.bNegated) return !RoomIsClean; // Negated
			return RoomIsClean;
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

	// Remove all that are prepared. Menos a SanitizationRoom
	Rooms.RemoveAll([](ARoomPreparation* Room) {
		return Room != nullptr && Room->IsRoomPrepared() && Room->Name != "SanitizationRoom";
	});

	// Evaluate G2
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

void AExperimentRoomPreparation::ExperimentFinished() {
	FValidationStruct Validation = GetValidationStruct();

	// Validate Experiment is a Assync method. So we need to wait it finished, and then we call Super:ExperimentFinished
	ValidateExperiment(Validation);
}

void AExperimentRoomPreparation::ExecuteMoveFurniture(FString RobotName, ARoomPreparation* Room) {
	NumberOfTask = 2;
	for (ARobotOrganizer* Organizer : OrganizersTeam) {
		Organizer->StartNewTask(ESkillEnum::MOVE_FURNITURE, Room);
	};
}

FValidationStruct AExperimentRoomPreparation::GetValidationStruct() {
	FValidationStruct Result;

	for (auto Organizer: OrganizersTeam) {
		Result.robots_organizer.Add(Organizer->RobotProperties.Name);
	}

	Result.Robots.Add(CleanerRobot->RobotProperties.Name);

	TArray<AActor*> RoomsOnMap;
	TArray<ARoomPreparation*> AllRooms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomPreparation::StaticClass(), RoomsOnMap);
	for (AActor* Actor : RoomsOnMap) {
		ARoomPreparation* Room = Cast<ARoomPreparation>(Actor);
		if (Room != nullptr) {
			AllRooms.Add(Room);
		}
	}

	// preenche o initial state das salas
	for (auto WorldRoom: WorldRoomsStruct) {
		Result.Rooms.Add(WorldRoom.Name);
		Result.door_open_inicial.Add(WorldRoom.Name, WorldRoom.bDoorOpen);
		Result.room_clean_initial_state.Add(WorldRoom.Name, WorldRoom.bIsClean);
		Result.room_organize_initial_state.Add(WorldRoom.Name, WorldRoom.bIsPrepared);
	}

	// preenche o final state das salas
	for (auto Room: AllRooms) {
		Result.door_open_final.Add(Room->Name, Room->DoorOpened);
		Result.room_clean_final_state.Add(Room->Name, Room->IsTrashClean());
		Result.room_organize_final_state.Add(Room->Name, Room->IsFurnitureOrganized());
	}

	TMap<FString, FRoomAssignment> CleaningAssignments;
	TMap<FString, FRoomAssignment> SanitizationTasks;

	TMap<FString, TArray<FString>> RoomsCleanedByRobot;
	for (const FTaskResult& TaskResult : CurrentExperiment.TaskResults) {
		if (TaskResult.TaskName == ESkillEnum::CLEAN_ROOM) {
			// Atualiza o mapa de limpeza
			RoomsCleanedByRobot.FindOrAdd(TaskResult.RobotName).Add(TaskResult.Location);

			FRoomAssignment& Assignment = CleaningAssignments.FindOrAdd(TaskResult.Location);
			Assignment.assigned.Add(TaskResult.RobotName);
		}
	}

	// check if the robot sanitize after clean
	for (const FTaskResult& TaskResult : CurrentExperiment.TaskResults) {
		if (TaskResult.TaskName == ESkillEnum::SANITIZE_ROBOT) {
			const FString& Robot = TaskResult.RobotName;

			if (RoomsCleanedByRobot.Contains(Robot)) {
				FRoomAssignment& SanitizationAssignment = SanitizationTasks.FindOrAdd(Robot);
				for (const FString& CleanedRoom : RoomsCleanedByRobot[Robot]) {
					SanitizationAssignment.assigned.AddUnique(CleanedRoom);
				}
			}
		}
	}

	Result.cleaning_assignments = CleaningAssignments;
	Result.sanitization_tasks = SanitizationTasks;

	Result.min_organizers = 2;
	Result.max_organizers = 4;

	return Result;
}

void AExperimentRoomPreparation::ValidateExperiment(FValidationStruct ValidationStruct) {
	FString RequestBody;

	if (!FJsonObjectConverter::UStructToJsonObjectString(ValidationStruct, RequestBody)) {
		UE_LOG(LogTemp, Error, TEXT("Erro ao converter ValidationStruct para JSON"));
		return;
	}

	// 🔍 Loga o JSON antes de enviar
	UE_LOG(LogTemp, Warning, TEXT("RequestBody JSON:\n%s"), *RequestBody);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(TEXT("http://127.0.0.1:8000/validar"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			if (bWasSuccessful && Response.IsValid()) {
				FString ResponseContent = Response->GetContentAsString();
				CurrentExperiment.ValidationResult = Response->GetContentAsString();
				this->HandleValidationSuccess(ResponseContent);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Erro na requisição HTTP"));
				if (Response.IsValid()) {
					UE_LOG(LogTemp, Error, TEXT("Código HTTP: %d"), Response->GetResponseCode());
					UE_LOG(LogTemp, Error, TEXT("Resposta: %s"), *Response->GetContentAsString());
					CurrentExperiment.ValidationResult = Response->GetContentAsString();
				}
				this->HandleValidationFailure();
			}
		}
	);
	Request->ProcessRequest();
}

// Seu método que será chamado após o sucesso da requisição
void AExperimentRoomPreparation::HandleValidationSuccess(const FString& ResponseContent) {
	UE_LOG(LogTemp, Log, TEXT("Validação HTTP bem-sucedida! Conteúdo recebido: %s"), *ResponseContent);


	Super::ExperimentFinished();
}

// Seu método que será chamado após a falha da requisição (opcional)
void AExperimentRoomPreparation::HandleValidationFailure() {
	UE_LOG(LogTemp, Error, TEXT("Falha na validação HTTP."));


	Super::ExperimentFinished();
}