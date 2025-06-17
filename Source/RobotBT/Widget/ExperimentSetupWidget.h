#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/EditableText.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "RobotBT/Experiments/MainExperimentInstance.h"
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

	// world knowledge json file path
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* WorldPath;

	// output json file path, provided by MutRose
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* OutputPath;

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

	// Final Result. It is a rich text. You can use <Green> value </> to print a Green text. And <Red> value </> to print a Red text. \n to break line
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* FinalResult;

	UFUNCTION(BlueprintCallable)
	void InitiateExperiment();

	UFUNCTION(BlueprintCallable)
	void PauseExperiment(bool NewValue);

	UFUNCTION(BlueprintCallable)
	void OpenFileClicked(FString Type);

	UFUNCTION(BlueprintCallable)
	FString ReadOutputPath(FString NewPath);

	UFUNCTION(BlueprintCallable)
	FString ReadWorldPath(FString NewPath);

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
	UMainExperimentInstance* ExperimentInstance = nullptr;

	UFUNCTION(BlueprintCallable)
	void RobotCameraComboSelected(FString RobotSelected);

	UFUNCTION(BlueprintCallable)
	void CaptureComboSelected(FString RobotSelected);

	UFUNCTION(BlueprintCallable)
	void HideRoof(bool NewValue);

	UFUNCTION(BlueprintCallable)
	bool IsLoading();

	UFUNCTION(BlueprintCallable)
	void GetLastExperimentResult();

	// salva o robo selecionado
	UPROPERTY(BlueprintReadOnly)
	FString RobotSelected = "Default";
};
