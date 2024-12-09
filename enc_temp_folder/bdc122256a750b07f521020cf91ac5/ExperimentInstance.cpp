#include "ExperimentInstance.h"

#include "Experiment.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Util/MyJsonWriter.h"

UExperimentInstance::UExperimentInstance() {
	
}

void UExperimentInstance::Init() {
	Super::Init();


}

void UExperimentInstance::StartNewExperiment(FExperimentResult Experiment) {
	Experiment.ExperimentId = 0;
	Experiment.WallClockInSeconds = 0;
	CurrentExperiment = Experiment;

	NextExperiment();
}

void UExperimentInstance::NextExperiment() {
	FExperimentResult Experiment;
	Experiment.ExperimentId = ExperimentId++;
	Experiment.Approach = CurrentExperiment.Approach;
	Experiment.ExperimentSpeed = CurrentExperiment.ExperimentSpeed;
	Experiment.WallClockInSeconds = 0;
	Experiment.RepeatExperimentFor = CurrentExperiment.RepeatExperimentFor;
	
	if (ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		FinishAllExperiment();
	}

	CurrentExperiment = Experiment;

	if (UWorld* World = GetWorld()) {
		ExperimentGameMode = Cast<AExperiment>(World->GetAuthGameMode());
		if (ExperimentGameMode != nullptr) {
			ExperimentGameMode->ExecuteExperiment(CurrentExperiment);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load AExperiment."));
		}
	}
}

FExperimentResult& UExperimentInstance::GetCurrentExperiment() {
	return CurrentExperiment;
}

float UExperimentInstance::GetTimer() {
	if (ExperimentGameMode != nullptr) {
		ExperimentGameMode->WallClockInSeconds;
		
	}
	return 0;

}

void UExperimentInstance::ExperimentFinished(FExperimentResult NewExperiment) {
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

void UExperimentInstance::OnLevelLoaded() {
	NextExperiment();
}

void UExperimentInstance::FinishAllExperiment() {
	if (CurrentExperiment.SaveResults) {
		UMyJsonWriter::AddToJsonFile(Experiments, CurrentExperiment.ExperimentName, CurrentExperiment.ScenarioId);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}