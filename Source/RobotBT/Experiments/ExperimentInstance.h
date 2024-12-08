#pragma once

#include "CoreMinimal.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "ExperimentInstance.generated.h"

UCLASS()
class UExperimentInstance : public UGameInstance {
	GENERATED_BODY()

public:
	UExperimentInstance();

	void Init() override;

	// used to indicate the Experiment Id. -1 will be a single experiment
	int32 ExperimentId = -1;

	// End Experiment Properties. 
	UFUNCTION()
	void FinishAllExperiment();

	// Current Experiment finished. Called by the Experiment
	UFUNCTION()
	void ExperimentFinished(FExperimentResult NewExperiment);

	UFUNCTION()
	void StartNewExperiment();

	FExperimentResult& GetCurrentExperiment();

	UPROPERTY()
	TArray<FExperimentResult> Experiments;

	UPROPERTY(EditAnywhere)
	FExperimentResult CurrentExperiment;



};


