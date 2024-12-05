// Copyright Epic Games, Inc. All Rights Reserved.

#include "Experiment.h"

#include "RobotBT/RobotBTPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "RobotBT/Util/MyJsonReader.h"
#include "RobotBT/Util/UtilMethods.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Util/MyCSVReader.h"

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

void AExperiment::BeginPlay() {
    Super::BeginPlay();

	// altera vecolidade pra ficar 10x mais rapido
	if (GetWorld()) {
		GetWorld()->GetWorldSettings()->SetTimeDilation(ExperimentSpeed);
	}


	if (SaveResults) {
		UMyCSVReader::CreateCSVFile(ExperimentName, ScenarioId);
	}
}

void AExperiment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

	// update the wall clock in seconds
	float CurrentTime = GetWorld()->GetTimeSeconds();
	Experiment.WallClockInSeconds = CurrentTime - ExperimentStartTime;

	if (Experiment.WallClockInSeconds > MaxWallClockInSeconds) {
		TimeIsOver();
	}
}

void AExperiment::ExecuteNextExperiment() {
	ExperimentStartTime = GetWorld()->GetTimeSeconds();

	// saves the result before continue
	if (SaveResults && Experiment.TaskResults.Num() > 0) {
		UMyCSVReader::AddToFile(Experiment);
	}

	ExperimentId++;
	if (ExperimentId >= RepeatExperimentFor) {
		FinishAllExperiment();
		return;
	}

	// Prepare World to match the world knowledge
	PrepareWorld();

	Experiment.ExperimentId = ExperimentId;
	Experiment.Approach = Approach;
	Experiment.WallClockInSeconds = 0;
	CurrentTaskIndex = -1;

	FString Message = FString::Printf(TEXT("Executing Experiment With Id: %d"), Experiment.ExperimentId);
	UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);
	CurrentTask = GetNextTask();
	ExecuteCurrentTask();
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
		ExecuteNextExperiment();
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTaskIterator is null!"));
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
	Experiment.WallClockInSeconds = GetWorld()->GetTimeSeconds() - ExperimentStartTime;
	Experiment.TaskResults.Add(TaskResult);

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
		UUtilMethods::PrintFailureMessage(TaskResult.FailureReasonEnum, TaskResult.InitialRobotsProperties.Name);
		ExecuteNextExperiment();
	}
}

void AExperiment::FinishAllExperiment() {
	UUtilMethods::ShowLogMessage("All Finished!!!", EMessageColorEnum::INFO);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

bool AExperiment::ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition) {
	return Predicate.Split(TEXT("."), &OutObjectName, &OutCondition);
}

void AExperiment::LoadTasksFromFile() {
	Tasks = UMyJsonReader::ReadTaskFromFile(ExperimentName, ScenarioId);
}

bool AExperiment::LoadWorldFromFile() {
	WorldRoomsStruct = UMyJsonReader::LoadWorldData(ExperimentName, ScenarioId);
	if (WorldRoomsStruct.Num() <= 0) {
		UUtilMethods::ShowLogMessage(TEXT("Failed to load world data"), EMessageColorEnum::ERROR);
		return false;
	}

	return true;
}