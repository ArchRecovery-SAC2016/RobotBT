#pragma once

#include "CoreMinimal.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "MainExperimentInstance.generated.h"

class AExperiment;

UCLASS()
class UMainExperimentInstance : public UGameInstance {
	GENERATED_BODY()

public:
	UMainExperimentInstance();

	virtual void Init() override;

	// used to indicate the Experiment Id.
	int32 ExperimentId = 0;

	// End Experiment Properties. 
	UFUNCTION()
	void FinishAllExperiment();

	// Start a new experiment. It is called by ExperimentSetupWidget
	UFUNCTION()
	void StartNewExperiment(FExperimentResult Experiment);

	// Current Experiment finished. Called by the Experiment
	UFUNCTION()
	void ExperimentFinished(FExperimentResult NewExperiment);

	UFUNCTION()
	void OnLevelLoaded();

	UFUNCTION()
	void NextExperiment();

	FExperimentResult& GetCurrentExperiment();

	UPROPERTY()
	TArray<FExperimentResult> Experiments;

	UPROPERTY()
	FExperimentResult CurrentExperiment;

	UFUNCTION()
	float GetTimer();
private:
	UPROPERTY()
	AExperiment* ExperimentGameMode;


};


