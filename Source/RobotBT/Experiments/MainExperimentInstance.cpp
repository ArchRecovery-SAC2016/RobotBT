#include "MainExperimentInstance.h"

#include "Experiment.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Util/MyJsonWriter.h"

UMainExperimentInstance::UMainExperimentInstance() {
	
}

void UMainExperimentInstance::Init() {
	Super::Init();
}

void UMainExperimentInstance::StartNewExperiment(FExperimentResult Experiment) {
	Experiment.ExperimentId = ExperimentId;
	Experiment.WallClockInSeconds = 0;
	CurrentExperiment = Experiment;

	if (UWorld* World = GetWorld()) {
		ExperimentGameMode = Cast<AExperiment>(World->GetAuthGameMode());
		ExperimentGameMode->OnExperimentFinished.AddDynamic(this, &UMainExperimentInstance::ExperimentFinished);

		if (ExperimentGameMode != nullptr) {
			ExperimentGameMode->ExecuteExperiment(CurrentExperiment);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load AExperiment."));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load World."));
	}

	NextExperiment();
}

void UMainExperimentInstance::NextExperiment() {
	FExperimentResult Experiment;
	Experiment.ExperimentId = ExperimentId;
	Experiment.Approach = CurrentExperiment.Approach;
	Experiment.ExperimentSpeed = CurrentExperiment.ExperimentSpeed;
	Experiment.WallClockInSeconds = 0;
	Experiment.RepeatExperimentFor = CurrentExperiment.RepeatExperimentFor;
	
	if (ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		FinishAllExperiment();
	}

	CurrentExperiment = Experiment;
	if (ExperimentGameMode != nullptr) {
		ExperimentGameMode->ExecuteExperiment(CurrentExperiment);
	}

}

void UMainExperimentInstance::ExperimentFinished(FExperimentResult NewExperiment) {
	Experiments.Add(NewExperiment);

	ExperimentId++;
	if (ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		FinishAllExperiment();
		return;
	}

	// restart the level
	if (UWorld* World = GetWorld()) {
		FName CurrentLevelName = World->GetFName();
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = __LINE__; // Número único para o callback.

		UGameplayStatics::LoadStreamLevel(World, CurrentLevelName, true, false, LatentInfo);
	}
}

void UMainExperimentInstance::OnLevelLoaded() {
	NextExperiment();
}

void UMainExperimentInstance::FinishAllExperiment() {
	if (CurrentExperiment.SaveResults) {
		UMyJsonWriter::AddToJsonFile(Experiments, CurrentExperiment.ExperimentName, CurrentExperiment.ScenarioId);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

float UMainExperimentInstance::GetTimer() {
	if (ExperimentGameMode != nullptr) {
		return  ExperimentGameMode->WallClockInSeconds;
	}
	return 0;
}

FExperimentResult& UMainExperimentInstance::GetCurrentExperiment() {
	return CurrentExperiment;
}