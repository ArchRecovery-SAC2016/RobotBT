#include "ExperimentSetupWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Experiments/ExperimentInstance.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Util/MyJsonReader.h"

void UExperimentSetupWidget::NativeConstruct() {
	Super::NativeConstruct();

	// set the default values for output path
	FString Path = "Data/RoomPreparation/Scenario_1/";
	FString OutputPathFile = FPaths::ProjectContentDir() + Path + "task_output.json";
	SetOutputPath(OutputPathFile);

	// set the default values for output path
	FString WorldPathFile = FPaths::ProjectContentDir() + Path + "world_db.json" ;
	SetWorldPath(WorldPathFile);

	// set the default values. It is the default values for the FExperimentResult
	ExperimentSpeed->SetText(FText::AsNumber(Experiment.ExperimentSpeed));
	RepeatExperimentFor->SetText(FText::AsNumber(Experiment.RepeatExperimentFor));
	MaxWallClockInSeconds->SetText(FText::AsNumber(Experiment.MaxWallClockInSeconds));
	
	BaselineApproach->SetCheckedState(ECheckBoxState::Checked);

	if (Experiment.SaveResults) {
		SaveResults->SetCheckedState(ECheckBoxState::Checked);
	} else {
		SaveResults->SetCheckedState(ECheckBoxState::Unchecked);
	}
}

void UExperimentSetupWidget::InitiateExperiment() {
	Experiment.ExperimentSpeed = FCString::Atof(*ExperimentSpeed->GetText().ToString());
	Experiment.RepeatExperimentFor = FCString::Atoi(*RepeatExperimentFor->GetText().ToString());
	Experiment.MaxWallClockInSeconds = FCString::Atoi(*MaxWallClockInSeconds->GetText().ToString());

	ExperimentIsValid = ValidateInputs();

	if (!ExperimentIsValid) return;

	UExperimentInstance* ExperimentInstance = Cast<UExperimentInstance>(GetWorld()->GetGameInstance());

	if (ExperimentIsValid && ExperimentInstance != nullptr) {
		ExperimentStarted = true;
		ExperimentInstance->StartNewExperiment(Experiment);
	} 
}

bool UExperimentSetupWidget::ValidateInputs() {
	if (Experiment.ExperimentSpeed == 0) {
		SetMessage("Experiment speed invalid.");
		return false;
	}

	if (Experiment.RepeatExperimentFor == 0) {
		SetMessage("RepeatExperimentFor invalid.");
		return false;
	}

	if (Experiment.MaxWallClockInSeconds == 0) {
		SetMessage("MaxWallClockInSeconds invalid.");
		return false;
	}

	if (Experiment.OutputJsonString == "") {
		SetMessage("Output Json invalid.");
		return false;
	}

	if (Experiment.WorldJsonString == "") {
		SetMessage("World Json invalid.");
		return false;
	}

	return true;
}

void UExperimentSetupWidget::PauseExperiment(bool NewValue) {
	UGameplayStatics::SetGamePaused(GetWorld(), NewValue);
}

void UExperimentSetupWidget::SetOutputPath(FString NewPath) {
	OutputPath->SetText(FText::FromString(NewPath));
	Experiment.OutputJsonString = UMyJsonReader::ReadStringFromFile(NewPath);
}

void UExperimentSetupWidget::SetWorldPath(FString NewPath) {
	WorldPath->SetText(FText::FromString(NewPath));
	Experiment.WorldJsonString = UMyJsonReader::ReadStringFromFile(NewPath);
}

void UExperimentSetupWidget::SetMessage(FString NewMessage) {
	Message->SetText(FText::FromString(NewMessage));


}






