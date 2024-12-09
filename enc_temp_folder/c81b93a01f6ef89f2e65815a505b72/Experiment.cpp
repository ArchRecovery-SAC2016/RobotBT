#include "Experiment.h"

#include "ExperimentInstance.h"
#include "RobotBT/RobotBTPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "RobotBT/Util/MyJsonReader.h"
#include "RobotBT/Util/UtilMethods.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

AExperiment::AExperiment()
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

void AExperiment::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (ExperimentStarted == false) return;

	// update the wall clock in seconds
	WallClockInSeconds = GetWorld()->GetTimeSeconds() - ExperimentStartTime;

	if (WallClockInSeconds > CurrentExperiment.MaxWallClockInSeconds) {
		TimeIsOver();
	}
}

void AExperiment::BeginPlay() {
    Super::BeginPlay();

}

void AExperiment::ExecuteExperiment(FExperimentResult& NewExperiment) {
	CurrentExperiment = NewExperiment;

	// change the speed of the world
	if (GetWorld()) {
		GetWorld()->GetWorldSettings()->SetTimeDilation(CurrentExperiment.ExperimentSpeed);
	}


	// Todo: Change this to the game instance
	LoadTasksFromFile();

	// Todo: Change this to the game instance
	LoadWorldFromFile();

	// Prepare World to match the world knowledge
	PrepareWorld();

	// TODO: REMOVE THE ROBOTS PROPERTIES FROM HERE AND GET FROM FILES 
	CurrentExperiment.Robots = RobotsProperties;
	CurrentTaskIndex = -1;

	FString Message = FString::Printf(TEXT("Executing Experiment With Id: %d"), CurrentExperiment.ExperimentId);
	UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);
	CurrentTask = GetNextTask();
	ExecuteCurrentTask();

	ExperimentStarted = true;
}

FTask* AExperiment::GetNextTask() {
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
		UUtilMethods::ShowLogMessage(TEXT("No task found! Experiment is over"), EMessageColorEnum::INFO);
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

void AExperiment::ExecuteCurrentTask() {
	if (CurrentTask == nullptr || (CurrentTask != nullptr && CurrentTask->Decomposition.Num() == 0)) {
		// se entrar aqui, entao acabou as tarefas.
		UE_LOG(LogTemp, Error, TEXT("[AExperiment::ExecuteCurrentTask] No more tasks found!"));
		ExperimentFinished();
		return;
	}

	// restart the decomposition index
	CurrentDecompositionIndex = 0;
	DecompositionQueue.Empty();

	// if is empty, so is the first time of this task, so we fill the decomposition queue
	for (auto it = CurrentTask->Decomposition.CreateConstIterator(); it; ++it) {
		DecompositionQueue.Add(it.Value());
	}

	ExecuteCurrentDecomposition();
}

void AExperiment::CurrentTaskFinished(FTaskResult TaskResult) {
	CurrentExperiment.TaskResults.Add(TaskResult);

	// if the task was successful, we can go to the next decomposition
	if (TaskResult.SuccessResult) {
		NumberOfTask--; // substract the number o task, because the organizer need 2 tasks

		if (NumberOfTask > 0) return;

		if (CurrentDecompositionIndex + 1 < DecompositionQueue.Num() ) {
			CurrentDecompositionIndex++;
			ExecuteCurrentDecomposition();
		} else {
			CurrentTask = GetNextTask();
			ExecuteCurrentTask();
		}
	} else {
		UUtilMethods::PrintFailureMessage(TaskResult.FailureReasonEnum, TaskResult.RobotName);
		ExperimentFinished();
	}
}

void AExperiment::ExperimentFinished() {
	UGameInstance* GameInstance = GetGameInstance();

	UExperimentInstance* ExperimentInstance = Cast<UExperimentInstance>(GameInstance);

	if (ExperimentInstance) {
		// Agora você pode acessar os métodos ou variáveis do URobotBTInstance
		ExperimentInstance->ExperimentFinished(CurrentExperiment);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("URobotBTInstance not found!"));
	}
}

bool AExperiment::ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition) {
	return Predicate.Split(TEXT("."), &OutObjectName, &OutCondition);
}

void AExperiment::LoadTasksFromFile() {
	Tasks = UMyJsonReader::ReadTaskFromFile(CurrentExperiment.ExperimentName, CurrentExperiment.ScenarioId);
}

bool AExperiment::LoadWorldFromFile() {
	WorldRoomsStruct = UMyJsonReader::LoadWorldData(CurrentExperiment.ExperimentName, CurrentExperiment.ScenarioId);
	if (WorldRoomsStruct.Num() <= 0) {
		UUtilMethods::ShowLogMessage(TEXT("Failed to load world data"), EMessageColorEnum::ERROR);
		return false;
	}

	return true;
}