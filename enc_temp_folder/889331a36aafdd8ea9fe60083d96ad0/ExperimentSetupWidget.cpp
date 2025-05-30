#include "ExperimentSetupWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Util/MyJsonReader.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "RobotBT/Actors/Robot.h"

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

	if (Experiment.SaveResults) {
		SaveResults->SetCheckedState(ECheckBoxState::Checked);
	} else {
		SaveResults->SetCheckedState(ECheckBoxState::Unchecked);
	}

	ExperimentInstance = Cast<UMainExperimentInstance>(GetWorld()->GetGameInstance());
	if (ExperimentInstance != nullptr) {
		if (ExperimentInstance->MustContinueExperiment) { // essa eh uma flag que indica se o experimento jah foi startado e precisa continuar
			ExperimentInstance->NextExperiment();
		}
	}

	// will update the FinalResult description with the last experiment result
	GetLastExperimentResult();
}

void UExperimentSetupWidget::InitiateExperiment() {
	Experiment.ExperimentSpeed = FCString::Atof(*ExperimentSpeed->GetText().ToString());
	Experiment.RepeatExperimentFor = FCString::Atoi(*RepeatExperimentFor->GetText().ToString());
	Experiment.MaxWallClockInSeconds = FCString::Atoi(*MaxWallClockInSeconds->GetText().ToString());

	if (SaveResults->GetCheckedState() == ECheckBoxState::Checked) Experiment.SaveResults = true;
	else Experiment.SaveResults = false;

	if (GenerateRandomProperties->GetCheckedState() == ECheckBoxState::Checked) Experiment.GenerateRandomProperties = true;
	else Experiment.GenerateRandomProperties = false;

	ExperimentIsValid = ValidateInputs();

	if (!ExperimentIsValid) return;

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

void UExperimentSetupWidget::RobotCameraSelected(FString RobotSelected) {
	TArray<AActor*> RobotsOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobot::StaticClass(), RobotsOnMap);

	if (RobotSelected == "Default") {
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		
		if (PlayerController) {
			APawn* PlayerPawn = PlayerController->GetPawn();
			if (PlayerPawn) {
				PlayerController->SetViewTargetWithBlend(PlayerPawn, 0.5f);
				HideRoof(true); // Certifique-se de que HideRoof(false) faz o que você espera
				return;
			}
		}
		return;
	}

	for (AActor* Actor : RobotsOnMap) {
		ARobot* Robot = Cast<ARobot>(Actor);
		if (Robot != nullptr && Robot->RobotProperties.Name == RobotSelected) {
			Robot->ActivateRobotCamera();
			HideRoof(false);
			return;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[UExperimentSetupWidget::RobotCameraSelected] No Robot whit name %s found"), *RobotSelected);
}

void UExperimentSetupWidget::HideRoof(bool NewValue) {
	// Load all Doors Sensors, so we can watch it
	TArray<AActor*> RoomsOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavigableArea::StaticClass(), RoomsOnMap);

	for (AActor* Actor : RoomsOnMap) {
		ANavigableArea* Room = Cast<ANavigableArea>(Actor);
		if (Room != nullptr) {
			Room->HideRoof(NewValue);
		}
		else {
			Room->HideRoof(NewValue);
		}
	}
}

bool UExperimentSetupWidget::IsLoading() {
	if (ExperimentInstance == nullptr) return false;

	return ExperimentInstance->IsLoading;
}

void UExperimentSetupWidget::GetLastExperimentResult() {
	if (ExperimentInstance == nullptr) return;

	FExperimentResult Result = ExperimentInstance->GetLastExperimentResult(); // i want to get the last one.

	if (Result.ResultFinal.ResultEnum == EnumResultFinal::NotProcessed) {
		FinalResult->SetText(FText::FromString("Failed to Processed Result. "));
	} else if (Result.ResultFinal.ResultEnum == EnumResultFinal::ValidationCallFailed) {
		FinalResult->SetText(FText::FromString("<Red> All Task Finished but Failed to Call Validation to check if all conditions are valid. </>"));
		
	} else if (Result.ResultFinal.ResultEnum == EnumResultFinal::ValidationResult) {
		FinalResult->SetText(FText::FromString("<Green> All Task Finished </>. Validation Result: " + Result.ResultFinal.Description));
	}
	else if (Result.ResultFinal.ResultEnum == EnumResultFinal::CausalAnalysisCallFailed) {
		FinalResult->SetText(FText::FromString("<Red> Task  Failed. Also failed to Call Causal Analysis to check</>"));
	}
	else if (Result.ResultFinal.ResultEnum == EnumResultFinal::CausalAnalysisResult) {
		FinalResult->SetText(FText::FromString("<Red> Task  Failed </>. Causal Analysis Result: " + Result.ResultFinal.Description));
	}
	else {
		FinalResult->SetText(FText::FromString("No Result Found"));
	}
}

void UExperimentSetupWidget::PauseExperiment(bool NewValue) {
	ExperimentPaused = NewValue;
	UGameplayStatics::SetGamePaused(GetWorld(), ExperimentPaused);
}

void UExperimentSetupWidget::OpenFileClicked(FString Type) {
	// Acessa a interface DesktopPlatform
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform) {
		// Define a janela para selecionar o arquivo
		TArray<FString> OutFileNames;
		const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

		// Filtra os tipos de arquivo desejados
		bool bFileSelected = DesktopPlatform->OpenFileDialog(
			ParentWindowHandle,
			TEXT("Select a File"),
			FPaths::ProjectContentDir(),
			TEXT(""),
			TEXT("Arquivos JSON (*.json)|*.json|Todos os Arquivos (*.*)|*.*"),
			EFileDialogFlags::None,
			OutFileNames
		);

		if (bFileSelected && OutFileNames.Num() > 0) {
			// Pega o primeiro arquivo selecionado
			FString SelectedFilePath = OutFileNames[0];
			UE_LOG(LogTemp, Log, TEXT("Arquivo selecionado: %s"), *SelectedFilePath);

			if (Type == "Output") SetOutputPath(SelectedFilePath);
			else if (Type == "World") SetWorldPath(SelectedFilePath);
			else if (Type == "Robots") SetRobotsPath(SelectedFilePath);
			else UE_LOG(LogTemp, Log, TEXT("Invalid Type. Type must be Output, World, Robots"));
		}
	}
}

void UExperimentSetupWidget::SetOutputPath(FString NewPath) {
	OutputPath->SetText(FText::FromString(NewPath)); 	// TODO: Validade file
	Experiment.OutputJsonString = UMyJsonReader::ReadStringFromFile(NewPath);
}

void UExperimentSetupWidget::SetWorldPath(FString NewPath) {
	WorldPath->SetText(FText::FromString(NewPath));
	Experiment.WorldJsonString = UMyJsonReader::ReadStringFromFile(NewPath);
}

void UExperimentSetupWidget::SetRobotsPath(FString NewPath) {
	WorldPath->SetText(FText::FromString(NewPath));
	Experiment.RobotsConfigJsonString = UMyJsonReader::ReadStringFromFile(NewPath);
}

void UExperimentSetupWidget::SetMessage(FString NewMessage) {
	Message->SetText(FText::FromString(NewMessage));
}

FText UExperimentSetupWidget::GetExperimentIdValue() {
	if (ExperimentInstance != nullptr) {
		return FText::AsNumber(ExperimentInstance->CurrentExperiment.ExperimentId);
	}

	return FText::FromString("");
}

FText UExperimentSetupWidget::GetTimerValue() {
	if (ExperimentInstance != nullptr) {
		float TimerValue = ExperimentInstance->GetTimer();
		FString FormattedTime = FString::Printf(TEXT("%.2f s"), TimerValue);
		return FText::FromString(FormattedTime);
	}

	return FText::FromString("Timer");
}