#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "RobotBT/Experiments/ExperimentInstance.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "ExperimentSetupWidget.generated.h"

/**
 * Show the properties of world experiment
 */
UCLASS()
class ROBOTBT_API UExperimentSetupWidget : public UUserWidget {
	GENERATED_BODY()

public:
	void NativeConstruct();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	bool ExperimentIsValid = true;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	bool ExperimentStarted = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	bool ExperimentPaused = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Message;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* ExperimentSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* RepeatExperimentFor;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* MaxWallClockInSeconds;

	// output json file path, provided by MutRose
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* OutputPath;

	// world knowledge json file path
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* WorldPath;

	// robots configuration file path
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* RobotsConfigurationPath;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* GenerateRandomProperties;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* SaveResults;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ExperimentId;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Timer;

	UFUNCTION(BlueprintCallable)
	void InitiateExperiment();

	UFUNCTION(BlueprintCallable)
	void PauseExperiment(bool NewValue);

	UFUNCTION(BlueprintCallable)
	void OpenFileClicked(FString Type);

	UFUNCTION(BlueprintCallable)
	void SetOutputPath(FString NewPath);

	UFUNCTION(BlueprintCallable)
	void SetWorldPath(FString NewPath);

	UFUNCTION(BlueprintCallable)
	void SetRobotsPath(FString NewPath);

	UFUNCTION(BlueprintCallable)
	void SetMessage(FString NewMessage);

	UFUNCTION(BlueprintCallable)
	FText GetExperimentIdValue();

	UFUNCTION(BlueprintCallable)
	FText GetTimerValue();

	UPROPERTY()
	FExperimentResult Experiment;

	bool ValidateInputs();

	UPROPERTY()
	UExperimentInstance* ExperimentInstance;

};
