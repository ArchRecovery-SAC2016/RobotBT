#pragma once

#include "CoreMinimal.h"
#include "Struct/ExperimentResult.h"
#include "RobotBTInstance.generated.h"

UCLASS()
class URobotBTInstance : public UGameInstance {
	GENERATED_BODY()

public:
	URobotBTInstance();

	// used to indicate the Experiment Id. -1 will be a single experiment
	int32 ExperimentId = -1;

	UFUNCTION()
	void FinishAllExperiment(bool SaveResults, FString ExperimentName, int32 ScenarioId);

	UFUNCTION()
	void ExperimentFinished(FExperimentResult NewExperiment);

	UPROPERTY()
	TArray<FExperimentResult> Experiments;

};


