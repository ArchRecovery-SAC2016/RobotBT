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

	// End Experiment Properties. 
	UFUNCTION()
	void FinishAllExperiment();

	// Start a new experiment. It is called by ExperimentSetupWidget
	UFUNCTION()
	void StartNewExperiment(FExperimentResult Experiment);

	UFUNCTION()
	void ExecuteExperiment(FExperimentResult& NewExperiment);

	// Current Experiment finished. Called by the Experiment
	UFUNCTION()
	void ExperimentFinished(FExperimentResult NewExperiment);

	// Reset the level. Used when 
	UFUNCTION()
	void ResetLevel();

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

	UPROPERTY()
	bool MustContinueExperiment = false;

private:
	/** Handle do Timer */
	FTimerHandle TimerHandle_CountSeconds;

	/** Função chamada a cada segundo */
	void IncrementSeconds();
};


