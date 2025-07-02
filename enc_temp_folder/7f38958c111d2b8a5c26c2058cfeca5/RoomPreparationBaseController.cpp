#include "RoomPreparationBaseController.h"
#include "RobotBT/Util/MyJsonReader.h"
#include "RobotBT/Util/UtilMethods.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "RobotBT/Actors/RoomPreparation/RoomPreparation.h"

ARoomPreparationBaseController::ARoomPreparationBaseController() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARoomPreparationBaseController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (ExperimentStarted == false) return;

	// update the wall clock in seconds
	WallClockInSeconds = GetWorld()->GetTimeSeconds() - ExperimentStartTime;

	if (WallClockInSeconds > CurrentExperiment.MaxWallClockInSeconds) {
		TimeIsOver();
	}
}

void ARoomPreparationBaseController::BeginPlay() {
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
			CleanersRobot.Add(Cleaner);
			Cleaner->OnTaskFinished.AddDynamic(this, &ARoomPreparationBaseController::CurrentTaskFinished);
		}
	}

	// Load organization robot
	TArray<AActor*> FoundOrganizer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobotOrganizer::StaticClass(), FoundOrganizer);
	for (AActor* Actor : FoundOrganizer) {
		ARobotOrganizer* Organizer = Cast<ARobotOrganizer>(Actor);
		if (Organizer != nullptr) {
			Organizer->OnTaskFinished.AddDynamic(this, &ARoomPreparationBaseController::CurrentTaskFinished);
			OrganizersTeam.Add(Organizer);
		}
	}

	if (CleanersRobot.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Cleaner Robot. Please add at least one. No task will be executed!"));
		return;
	}

	if (OrganizersTeam.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("Failed do instantiate Organizer Team. Please add at least one. No task will be executed!"));
	}
}

void ARoomPreparationBaseController::ExecuteExperiment(FExperimentResult& NewExperiment) {
	// Prepare World to match the world knowledge
	PrepareWorld(NewExperiment.WorldJsonString);

	// Load tasks from file
	LoadTasksFromFile(NewExperiment.OutputTasksJsonString);

	CurrentExperiment = NewExperiment;
	ExperimentStartTime = GetWorld()->GetTimeSeconds();

	FetchRoomsToBePrepared();

	// change the speed of the world
	if (GetWorld()) {
		GetWorld()->GetWorldSettings()->SetTimeDilation(CurrentExperiment.ExperimentSpeed);
	}

	//	CurrentExperiment.Robots = RobotsProperties;
	CurrentTaskIndex = -1;

	FString Message = FString::Printf(TEXT("Executing RoomPreparationBaseController With Id: %d"), CurrentExperiment.ExperimentId);
	UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);
	CurrentTask = GetNextTask();
	ExecuteCurrentTask();

	ExperimentStarted = true;
}

FTask* ARoomPreparationBaseController::GetNextTask() {
	FTask* NewTask = nullptr;
	CurrentTaskIndex++;

	// Check if the tasks map is not empty
	if (Tasks.Num() != 0 && Tasks.Num() > CurrentTaskIndex) {
		TArray<FString> Keys;
		Tasks.GetKeys(Keys);

		FTask* Task = Tasks.Find(Keys[CurrentTaskIndex]);

		if (Task != nullptr) {
			NewTask = Task;
		}
	}

	if (NewTask == nullptr) {
		UUtilMethods::ShowLogMessage(TEXT("No task found! RoomPreparationBaseController is over"), EMessageColorEnum::INFO);
		return nullptr;
	}

	FString Message = FString::Printf(TEXT("Next Task Id: %s name: %s"), *NewTask->Id, *NewTask->Name);
	UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);

	if (CheckPreCondition(NewTask)) {
		return NewTask;
	}
	// if fails, we try another calling recursively. Will stop when the task is null
	return GetNextTask();
}

void ARoomPreparationBaseController::ExecuteCurrentTask() {
	if (CurrentTask == nullptr || (CurrentTask != nullptr && CurrentTask->Decomposition.Num() == 0)) {
		// se entrar aqui, entao acabou as tarefas.
		UE_LOG(LogTemp, Error, TEXT("[ARoomPreparationBaseController::ExecuteCurrentTask] No more tasks found!"));
		ExperimentFinished();
		return;
	}

	// restart the decomposition index
	CurrentDecompositionIndex = 0;
	DecompositionQueue.Empty();

	// if is empty, so is the first time of this task, so we fill the decomposition queue
	for (int32 i = 0; i < CurrentTask->Decomposition.Num(); i++) {
		DecompositionQueue.Add(CurrentTask->Decomposition[i]);
	}

	ExecuteCurrentDecomposition();
}

void ARoomPreparationBaseController::ExecuteCurrentDecomposition() {
	// A decomposicao esta bem confusa. Ela esta com os argumentos incompletos. Entao vamos usar apenas o nome e pegamos o local das tasks. 
	const FTaskDecomposition& CurrentDecomposition = DecompositionQueue[CurrentDecompositionIndex];
	FString RoomName = CurrentTask->Locations;

	ARoomPreparation* RoomLocation = GetRoomByName(RoomName);
	if (RoomLocation == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Room not found!"));
		return;
	}

	ESkillEnum SkillRequired = ESkillEnum::NONE;
	if (CurrentDecomposition.Name == "sanitize-robot") {
		SkillRequired = ESkillEnum::SANITIZE_ROBOT;
	}
	else if (CurrentDecomposition.Name == "clean-room") {
		SkillRequired = ESkillEnum::CLEAN_ROOM;
	}
	else if (CurrentDecomposition.Name == "open-door") {
		SkillRequired = ESkillEnum::OPEN_DOOR;
	}
	else if (CurrentDecomposition.Name == "move-furniture") {
		SkillRequired = ESkillEnum::MOVE_FURNITURE;
	} else {
		UE_LOG(LogTemp, Error, TEXT("Unknown Task Name: %s"), *CurrentDecomposition.Name);
		ExperimentFinished();
		return;
	}

	// Get a random cleaner, because is always one and the same
	if (CurrentExperiment.ControllerApproach == EControllerApproach::Baseline) {
		CleanerRobotInst = GetRandomCleanerRobot(SkillRequired);
	}else {
		CleanerRobotInst = GetBestCleanerRobot(SkillRequired);
	}


	if (CurrentDecomposition.Name == "sanitize-robot") {
		ExecuteSanitizeRobot(CleanerRobotInst, RoomLocation);
	} else if (CurrentDecomposition.Name == "clean-room") {
		ExecuteClean(CleanerRobotInst, RoomLocation);
	} else if (CurrentDecomposition.Name == "open-door") {
		ExecuteOpenDoor(CleanerRobotInst, RoomLocation);
	} else if (CurrentDecomposition.Name == "move-furniture") {
		FString RobotName = "OrganizerTeam";

		ExecuteMoveFurniture(RobotName, RoomLocation);
	}
}

ARobot* ARoomPreparationBaseController::GetRobotByName(FString RobotName) {
	TArray<ARobot*> RobotsOnMap = GetAllRobots();

	for (ARobot* Actor : RobotsOnMap) {
		if (Actor->RobotProperties.Name == RobotName) {
			return Actor;
		}
	}

	return nullptr;
}

TArray<ARobot*> ARoomPreparationBaseController::GetAllRobots() {
	TArray<AActor*> RobotsOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobot::StaticClass(), RobotsOnMap);

	TArray<ARobot*> Robots;
	for (AActor* Actor : RobotsOnMap) {
		ARobot* Robot = Cast<ARobot>(Actor);
		if (Robot == nullptr) continue;
		Robots.Add(Robot);
	}

	return Robots;
}

ARobotCleaner* ARoomPreparationBaseController::GetRandomCleanerRobot(ESkillEnum SkillRequired) {
	TArray<ARobotCleaner*> AvailableCleaners;

	// Filtra os robôs disponíveis
	for (ARobotCleaner* CleanerRobot : CleanersRobot) {
		if (CleanerRobot && CleanerRobot->TaskAllocated == ESkillEnum::NONE) {
			AvailableCleaners.Add(CleanerRobot);
		}
	}

	// Se não houver robôs disponíveis, retorna nullptr
	if (AvailableCleaners.Num() == 0) {
		return nullptr;
	}

	// Escolhe um robô aleatório da lista de disponíveis
	int32 Index = UKismetMathLibrary::RandomInteger(AvailableCleaners.Num());
	return AvailableCleaners[Index];
}



ARobotOrganizer* ARoomPreparationBaseController::GetOrganizerRobot() {
	// TODO: Implementar lógica para escolher o melhor robô organizador
	int32 Index = UKismetMathLibrary::RandomInteger(OrganizersTeam.Num());
	return OrganizersTeam[Index];
}

ARobotCleaner* ARoomPreparationBaseController::GetBestCleanerRobot(ESkillEnum SkillRequired) {
	ARobotCleaner* BestRobot = nullptr;
	float HighestBattery = -1.0f;

	for (ARobotCleaner* CleanerRobot : CleanersRobot) {
		if (!CleanerRobot || CleanerRobot->TaskAllocated == ESkillEnum::NONE) { // if already doing a task, skip it
			continue;
		}

		const FRobotProperties& Properties = CleanerRobot->RobotProperties;

		// Verifica se o robô possui a skill requerida
		bool bHasSkill = false;
		for (const FSkill& Skill : Properties.Skills) {
			if (Skill.SkillEnum == SkillRequired) {
				bHasSkill = true;
				break;
			}
		}

		if (!bHasSkill) {
			continue;
		}

		// Considera esse robô se ele tiver mais bateria que o atual melhor
		float CurrentBattery = Properties.Battery.Charge;
		if (CurrentBattery > HighestBattery) {
			HighestBattery = CurrentBattery;
			BestRobot = CleanerRobot;
		}

		// Todo: considerar o mais proximo tbm
	}

	return BestRobot;

}

void ARoomPreparationBaseController::CurrentTaskFinished(FTaskResult TaskResult) {
	CurrentExperiment.WallClockInSeconds = WallClockInSeconds;
	CurrentExperiment.TaskResults.Add(TaskResult);

	// if the task was successful, we can go to the next decomposition
	if (TaskResult.SuccessResult) {
		NumberOfTask--; // substract the number o task, because the organizer need 2 tasks

		if (NumberOfTask > 0) return;

		if (CurrentDecompositionIndex + 1 < DecompositionQueue.Num()) {
			CurrentDecompositionIndex++;
			ExecuteCurrentDecomposition();
		}
		else {
			CurrentTask = GetNextTask();
			ExecuteCurrentTask();
		}
	}
	else {
		UUtilMethods::PrintFailureMessage(TaskResult.FailureReasonEnum, TaskResult.RobotName);
		ExperimentFinished();
	}
}

void ARoomPreparationBaseController::ExperimentFinished() {
	FOnPreparationFinish.Broadcast(CurrentExperiment);
	ExperimentStarted = false;
}

bool ARoomPreparationBaseController::CheckPreCondition(FTask* NewTask) {
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

bool ARoomPreparationBaseController::ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition) {
	return Predicate.Split(TEXT("."), &OutObjectName, &OutCondition);
}

void ARoomPreparationBaseController::PrepareWorld(FString WorldJsonString) {
	UE_LOG(LogTemp, Display, TEXT("Preparing the world for RoomPreparationBaseController: %d"), CurrentExperiment.ExperimentId);
	
	WorldRoomsStruct = UMyJsonReader::LoadWorldData(WorldJsonString);

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

	for (auto* CleanerRobot : CleanersRobot) {
		CleanerRobot->Initiate(CurrentExperiment.GenerateRandomProperties);
		RobotsProperties.Add(CleanerRobot->RobotProperties);
	}


	for (auto* Organizer : OrganizersTeam) {
		Organizer->Initiate(CurrentExperiment.GenerateRandomProperties);
		RobotsProperties.Add(Organizer->RobotProperties);
	}
}

void ARoomPreparationBaseController::TimeIsOver() {
	for (auto* CleanerRobot : CleanersRobot) {
		CleanerRobot->TimeIsOver();
	}

	for (auto* Organizer : OrganizersTeam) {
		Organizer->TimeIsOver();
	}
}

bool ARoomPreparationBaseController::LoadTasksFromFile(FString NewTasksJsonString) {
	Tasks = UMyJsonReader::LoadTaskData(NewTasksJsonString);
	if (Tasks.IsEmpty()) {
		UUtilMethods::ShowLogMessage(TEXT("Failed to load tasks json data"), EMessageColorEnum::ERROR);
		return false;
	}

	return true;
}

bool ARoomPreparationBaseController::LoadWorldFromFile(FString WorldJsonString) {
	WorldRoomsStruct = UMyJsonReader::LoadWorldData(WorldJsonString);
	if (WorldRoomsStruct.IsEmpty()) {
		UUtilMethods::ShowLogMessage(TEXT("Failed to load world data"), EMessageColorEnum::ERROR);
		return false;
	}

	return true;
}

void ARoomPreparationBaseController::FetchRoomsToBePrepared() {
	// Remove all that are prepared. Menos a SanitizationRoom
	Rooms.RemoveAll([](ARoomPreparation* Room) {
		return Room != nullptr && Room->IsRoomPrepared() && Room->Name != "SanitizationRoom";
		});
}

ARoomPreparation* ARoomPreparationBaseController::GetRoomByName(FString DoorName) {
	for (auto Room : Rooms) {
		if (Room->Name == DoorName) {
			return Room;
		}
	}

	return nullptr;
}

void ARoomPreparationBaseController::ExecuteClean(ARobotCleaner* CleanerRobot, ARoomPreparation* Room) {
	if (CleanerRobot == nullptr || Room == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("CleanerRobot or Room is null!"));
		return;
	}

	NumberOfTask = 1;
	CleanerRobot->StartNewTask(ESkillEnum::CLEAN_ROOM, Room);
}

void ARoomPreparationBaseController::ExecuteOpenDoor(ARobotCleaner* CleanerRobot,  ARoomPreparation* Room) {
	if (CleanerRobot == nullptr || Room == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("CleanerRobot or Room is null!"));
		return;
	}

	NumberOfTask = 1;
	CleanerRobot->StartNewTask(ESkillEnum::OPEN_DOOR, Room);
}

void ARoomPreparationBaseController::ExecuteSanitizeRobot(ARobotCleaner* CleanerRobot, ARoomPreparation* Room) {
	if (CleanerRobot == nullptr || Room == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("CleanerRobot or Room is null!"));
		return;
	}

	NumberOfTask = 1;
	CleanerRobot->StartNewTask(ESkillEnum::SANITIZE_ROBOT, Room);
}

bool ARoomPreparationBaseController::EvaluatePreCondition(const FPredicate& Predicate) {
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
			if (Predicate.bNegated) return !CleanerRobotInst->IsSanitized(); // Negated
			return CleanerRobotInst->IsSanitized();
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Unhandled Predicate: %s"), *Predicate.Condition);
	return false;
}


void ARoomPreparationBaseController::ExecuteMoveFurniture(FString RobotName, ARoomPreparation* Room) {
	// aki eu tenho que ver quantos 

	NumberOfTask = 0;
	for (auto Furniture: Room->Furnitures) {
		if (!Furniture->InPlace) {
			GetOrganizerRobot()->FurnitureToMoveLocation.Add(Furniture->GetActorLocation());
			NumberOfTask++;
		}
	}

	// manda todos. Se nao tiver nenhum movel alocado, entao nao faz nada
	for (ARobotOrganizer* Organizer : OrganizersTeam) {
		Organizer->StartNewTask(ESkillEnum::MOVE_FURNITURE, Room);
	};
}



FValidationStruct ARoomPreparationBaseController::GetValidationStruct() {
	FValidationStruct Result;

	for (auto Organizer : OrganizersTeam) {
		Result.robots_organizer.Add(Organizer->RobotProperties.Name);
	}

	Result.Robots.Add(CleanerRobotInst->RobotProperties.Name);

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
	for (auto WorldRoom : WorldRoomsStruct) {
		Result.Rooms.Add(WorldRoom.Name);
		Result.door_open_inicial.Add(WorldRoom.Name, WorldRoom.bDoorOpen);
		Result.room_clean_initial_state.Add(WorldRoom.Name, WorldRoom.bIsClean);
		Result.room_organize_initial_state.Add(WorldRoom.Name, WorldRoom.bIsPrepared);
	}

	// preenche o final state das salas
	for (auto Room : AllRooms) {
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

