#include "RobotBTInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Util/MyJsonWriter.h"

URobotBTInstance::URobotBTInstance() {

}

void URobotBTInstance::FinishAllExperiment(bool SaveResults, FString ExperimentName, int32 ScenarioId) {
	if (SaveResults) {
		UMyJsonWriter::AddToJsonFile(Experiments, ExperimentName, ScenarioId);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void URobotBTInstance::ExperimentFinished(FExperimentResult NewExperiment) {
	Experiments.Add(NewExperiment);
}
