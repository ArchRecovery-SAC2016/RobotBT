#include "MainExperimentInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Controllers/RoomPreparationBaseController.h"
#include "RobotBT/Util/MyJsonWriter.h"

UMainExperimentInstance::UMainExperimentInstance() {
	
}

void UMainExperimentInstance::Init() {
	Super::Init();

}

void UMainExperimentInstance::StartNewExperiment(FExperimentResult Experiment) {
	Experiments.Empty();

	Experiment.ExperimentId = 0;
	Experiment.WallClockInSeconds = 0;
	CurrentExperiment = Experiment;

	if (UWorld* World = GetWorld()) {
		ExecuteExperiment(CurrentExperiment);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load World."));
	}

	// Inicia o timer de segundos
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_CountSeconds,                   // Handle
		this,                                         // Objeto
		&UMainExperimentInstance::IncrementSeconds,   // Função a ser chamada
		0.01f,                                         // Intervalo em segundos
		true                                          // Repetir
	);

	NextExperiment();
}

void UMainExperimentInstance::NextExperiment() {
	CurrentExperiment.ExperimentId++;
	CurrentExperiment.WallClockInSeconds = 0;
	
	if (CurrentExperiment.ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		FinishAllExperiment();
	}
		
	ExecuteExperiment(CurrentExperiment);
}

void UMainExperimentInstance::ExecuteExperiment(FExperimentResult& NewExperiment) {
	ARoomPreparationBaseController* Controller = GetWorld()->SpawnActor<ARoomPreparationBaseController>();
	Controller->ExecuteExperiment(NewExperiment);

	Controller->FOnPreparationFinish.AddDynamic(this, &UMainExperimentInstance::ExperimentFinished);
}


void UMainExperimentInstance::ExperimentFinished(FExperimentResult NewExperiment) {
	Experiments.Add(NewExperiment);

	if (CurrentExperiment.ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		FinishAllExperiment();
		return;
	}

	ResetLevel();
}

void UMainExperimentInstance::ResetLevel() {
	UWorld* World = GetWorld();
	if (World) {
		FName CurrentLevelName = FName(*World->GetName());
		UGameplayStatics::OpenLevel(World, CurrentLevelName, false);
	}

	NextExperiment();
}

void UMainExperimentInstance::OnLevelLoaded() {
	NextExperiment();
}

void UMainExperimentInstance::FinishAllExperiment() {
	if (CurrentExperiment.SaveResults) {
		UMyJsonWriter::AddToJsonFile(Experiments, CurrentExperiment.ExperimentName, CurrentExperiment.ScenarioId);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

float UMainExperimentInstance::GetTimer() {
	return CurrentExperiment.WallClockInSeconds;
}

void UMainExperimentInstance::IncrementSeconds() {
	CurrentExperiment.WallClockInSeconds += 0.01f;
}

FExperimentResult& UMainExperimentInstance::GetCurrentExperiment() {
	return CurrentExperiment;
}
