#include "ExperimentInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Util/MyJsonWriter.h"

UExperimentInstance::UExperimentInstance() {
	
}

void UExperimentInstance::Init() {
	Super::Init();

	FExperimentResult Experiment;
	Experiment.ExperimentId = 0;
	Experiment.Approach = CurrentExperiment.Approach;
	Experiment.WallClockInSeconds = 0;

	CurrentExperiment = Experiment;
}

void UExperimentInstance::StartNewExperiment() {
	FExperimentResult Experiment;
	Experiment.ExperimentId = ExperimentId++;
	Experiment.Approach = CurrentExperiment.Approach;
	Experiment.WallClockInSeconds = 0;

	if (ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		FinishAllExperiment();
	}

	CurrentExperiment = Experiment;

	if (UWorld* World = GetWorld()) {
		FName CurrentLevelName = World->GetFName();
		UGameplayStatics::OpenLevel(World, CurrentLevelName, true);
	}
}

FExperimentResult& UExperimentInstance::GetCurrentExperiment() {
	return CurrentExperiment;
}

void UExperimentInstance::ExperimentFinished(FExperimentResult NewExperiment) {
	Experiments.Add(NewExperiment);

	ExperimentId++;
	if (ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		FinishAllExperiment();
	}

	StartNewExperiment();
}

void UExperimentInstance::FinishAllExperiment() {
	if (CurrentExperiment.SaveResults) {
		UMyJsonWriter::AddToJsonFile(Experiments, CurrentExperiment.ExperimentName, CurrentExperiment.ScenarioId);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}