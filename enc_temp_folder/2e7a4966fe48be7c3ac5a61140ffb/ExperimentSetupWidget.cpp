#include "ExperimentSetupWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Experiments/ExperimentInstance.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Util/MyJsonReader.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

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
		PauseExperiment(false);
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
	ExperimentPaused = NewValue;
	UGameplayStatics::SetGamePaused(GetWorld(), ExperimentPaused);
}

void UExperimentSetupWidget::SetOutputPath(FString NewPath) {
	OutputPath->SetText(FText::FromString(NewPath));
	Experiment.OutputJsonString = UMyJsonReader::ReadStringFromFile(NewPath);
}

void UExperimentSetupWidget::OpenFileClicked(FString Type) {
	FString SelectedFilePath;

	// Acessa a interface DesktopPlatform
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform) {
		// Define a janela para selecionar o arquivo
		TArray<FString> OutFileNames;
		const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

		// Filtra os tipos de arquivo desejados
		bool bFileSelected = DesktopPlatform->OpenFileDialog(
			ParentWindowHandle,
			TEXT("Selecione um Arquivo"),
			FPaths::ProjectContentDir(),
			TEXT(""),
			TEXT("Arquivos JSON (*.json)|*.json|Todos os Arquivos (*.*)|*.*"),
			EFileDialogFlags::None,
			OutFileNames
		);

		if (bFileSelected && OutFileNames.Num() > 0) {
			// Pega o primeiro arquivo selecionado
			SelectedFilePath = OutFileNames[0];
			UE_LOG(LogTemp, Log, TEXT("Arquivo selecionado: %s"), *SelectedFilePath);

			// Carrega o conteúdo do arquivo como exemplo (opcional)
			FString FileContents;
			if (FFileHelper::LoadFileToString(FileContents, *SelectedFilePath)) {
				UE_LOG(LogTemp, Log, TEXT("Conteúdo do Arquivo: %s"), *FileContents);
				// Chame sua lógica para processar o arquivo aqui
			}
		}
	}
}

void UExperimentSetupWidget::SetWorldPath(FString NewPath) {
	WorldPath->SetText(FText::FromString(NewPath));
	Experiment.WorldJsonString = UMyJsonReader::ReadStringFromFile(NewPath);
}

void UExperimentSetupWidget::SetMessage(FString NewMessage) {
	Message->SetText(FText::FromString(NewMessage));


}






