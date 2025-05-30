#pragma once

#include "CoreMinimal.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Struct/ValidationStruct.h"
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
	void ValidateExperiment(FValidationStruct ValidationStruct);

	UFUNCTION()
	void HandleValidation(const FString& ResponseContent, bool Success);

	UFUNCTION()
	void NextExperiment();

	FExperimentResult& GetCurrentExperiment();

	UPROPERTY()
	TArray<FExperimentResult> Experiments;

	UPROPERTY(BlueprintReadOnly, Category = "Experiment")
	FExperimentResult CurrentExperiment;

	UFUNCTION()
	float GetTimer();

	UPROPERTY()
	bool MustContinueExperiment = false;

	// used to control async methods, like the validation
	UPROPERTY(BlueprintReadOnly)
	bool IsLoading = false;

	UFUNCTION(BlueprintCallable, Category = "Experiment")
	FExperimentResult GetExperimentById(int32 Id) const;

private:
	/** Handle do Timer */
	FTimerHandle TimerHandle_CountSeconds;

	/** Função chamada a cada segundo */
	void IncrementSeconds();

	/* saves witch controller is running */
	UPROPERTY()
	class ARoomPreparationBaseController* CurrentController = nullptr;
};


