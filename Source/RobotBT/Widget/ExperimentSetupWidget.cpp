#include "ExperimentSetupWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Util/MyJsonReader.h"

void UExperimentSetupWidget::NativeConstruct() {
	Super::NativeConstruct();

	// set the default values for output path
    FString RoomPreparationPath = "Data/RoomPreparation/Scenario_1/task_output.json";
	FString Path = RoomPreparationPath;
	FString FilePath = FPaths::ProjectContentDir() + Path;
	SetOutputPath(FilePath);

	// set the default values for output path
	ExperimentSpeed->SetText(FText::FromString("2"));
	RepeatExperimentFor->SetText(FText::FromString("10"));
	MaxWallClockInSeconds->SetText(FText::FromString("140"));
	BaselineApproach->SetCheckedState(ECheckBoxState::Checked);
	SaveResults->SetCheckedState(ECheckBoxState::Checked);

}

void UExperimentSetupWidget::InitiateExperiment() {
	Experiment.ExperimentSpeed = FCString::Atof(*ExperimentSpeed->GetText().ToString());
	Experiment.RepeatExperimentFor = FCString::Atoi(*RepeatExperimentFor->GetText().ToString());
	Experiment.MaxWallClockInSeconds = FCString::Atoi(*MaxWallClockInSeconds->GetText().ToString());

	if (ExperimentIsValid) {

	} else {
		SetMessage("Experiment is not valid. Please, check the paths.");
	}
}

void UExperimentSetupWidget::PauseExperiment(bool NewValue) {
	
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




