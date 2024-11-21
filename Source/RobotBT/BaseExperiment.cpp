// Copyright Epic Games, Inc. All Rights Reserved.

#include "BaseExperiment.h"
#include "RobotBTPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Struct/RobotProperties.h"
#include "UObject/ConstructorHelpers.h"
#include "Util/MyJsonReader.h"
#include "Util/UtilMethods.h"

ABaseExperiment::ABaseExperiment()
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


void ABaseExperiment::BeginPlay() {
    Super::BeginPlay();

	// load all tasks from file
	LoadTasksFromFile();

	CurrentTask = GetNextTask();

	ExecuteCurrentTask();
}

void ABaseExperiment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

}

void ABaseExperiment::LoadTasksFromFile() {
	Tasks = UMyJsonReader::ReadJsonFile();
}

FTask* ABaseExperiment::GetNextTask() {
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


void ABaseExperiment::ExecuteCurrentTask() {
	if (CurrentTask == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTaskIterator is null!"));
		return;
	}

	if (CurrentTask != nullptr && CurrentTask->Decomposition.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] CurrentTask.Decomposition is empty!"));
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

void ABaseExperiment::ExecuteCurrentDecomposition() {
	// the implementation of this method is in the child class

}

bool ABaseExperiment::CheckPreCondition(FTask* NewTask) {
	// quem implementa isso eh o filho
	return true;
}


bool ABaseExperiment::ParsePredicate(const FString& Predicate, FString& OutObjectName, FString& OutCondition) {
	return Predicate.Split(TEXT("."), &OutObjectName, &OutCondition);
}

void ABaseExperiment::CurrentTaskFinished(FRobotProperties RobotProperties) {
	if (CurrentDecompositionIndex + 1 < DecompositionQueue.Num()) {
		CurrentDecompositionIndex++;
		ExecuteCurrentDecomposition();
	}
	else {
		CurrentTask = GetNextTask();
		ExecuteCurrentTask();
		
	}
}

void ABaseExperiment::CurrentTaskFailed(EFailureReasonEnum FailureReason, FRobotProperties RobotProperties) {
	UUtilMethods::PrintFailureMessage(FailureReason, RobotProperties);
	ExperimentIsOver = true;

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}