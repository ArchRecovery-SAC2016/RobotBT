// Copyright Epic Games, Inc. All Rights Reserved.

#include "BaseExperiment.h"
#include "RobotBTPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Util/MyJsonReader.h"
#include "Util/UtilMethods.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

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

}

void ABaseExperiment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

	Experiment.ExperimentTime += DeltaTime;

	
}

void ABaseExperiment::LoadTasksFromFile(FString ExperimentFolderName, int32 ScenarioId) {
	Tasks = UMyJsonReader::ReadTaskFromFile(ExperimentFolderName, ScenarioId);
}

FTask* ABaseExperiment::GetNextTask() {
	 // Check if the tasks map is not empty
	 if (Tasks.Num() != 0 && Tasks.Num() > CurrentTaskIndex) {
		 TArray<FString> Keys;
		 Tasks.GetKeys(Keys);

	 	FTask* Task = Tasks.Find(Keys[CurrentTaskIndex]);

		if (Task != nullptr) {
			// if found, we increment the index, and return
			CurrentTaskIndex++;
			return Task;
		}
	 }

	 UUtilMethods::ShowLogMessage(TEXT("No task found! Experiment is over"), EMessageColorEnum::ERROR);
	 ExperimentIsOver = true;
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

void ABaseExperiment::CurrentTaskFinished(FTaskResult TaskResult) {
	Experiment.TaskResults.Add(TaskResult);

	// if the task was successful, we can go to the next decomposition
	if (TaskResult.SuccessResult) {
		NumberOfTask--; // substract the number o task, because the organizer need 2 tasks

		if (NumberOfTask > 0) return;

		if (CurrentDecompositionIndex + 1 < DecompositionQueue.Num() ) {
			CurrentDecompositionIndex++;
			ExecuteCurrentDecomposition();
		}
		else {
			CurrentTask = GetNextTask();
			ExecuteCurrentTask();
		}
	} else {
		UUtilMethods::PrintFailureMessage(TaskResult.FailureReasonEnum, TaskResult.EndRobotsProperties);
		ExperimentIsOver = true;
	}
}