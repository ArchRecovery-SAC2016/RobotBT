#include "MainExperimentInstance.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
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
	CurrentController = GetWorld()->SpawnActor<ARoomPreparationBaseController>();
	CurrentController->ExecuteExperiment(NewExperiment);

	CurrentController->FOnPreparationFinish.AddDynamic(this, &UMainExperimentInstance::ExperimentFinished);
}

void UMainExperimentInstance::ExperimentFinished(FExperimentResult NewExperiment) {
	FTaskResult LastResult = NewExperiment.TaskResults.Last();

	if (LastResult.SuccessResult) {
		// TODO: Individualizar isso aki. tah muito voltado pro RoomPreparation. Talvez a validacao deve estar lah mesmo. ou entao, lah deve retornar tudo que eh necessario como o RequestBody e a url e aki soh chamar
		// o Validation Struct eh o pai, e tem FValidationRoomPreparationStruct filho desse validation struct
		FValidationStruct Validation = CurrentController->GetValidationStruct();
		ValidateExperiment(Validation);
	} else {
		CurrentExperiment.ResultFinal.FinalResult = EnumResultFinal::CausalAnalysisCallFailed;
		CurrentExperiment.ResultFinal.Description = EFailureReasonEnumHelper::GetDisplayName(LastResult.FailureReasonEnum);
		Experiments.Add(CurrentExperiment);
	}

	// adiciona na lista
	if (CurrentExperiment.ExperimentId >= CurrentExperiment.RepeatExperimentFor) {
		MustContinueExperiment = false;
		FinishAllExperiment();
		return;
	}
	
	MustContinueExperiment = true;
}

void UMainExperimentInstance::ResetLevel() {
	UWorld* World = GetWorld();
	if (World) {
		FName CurrentLevelName = FName(*World->GetName());
		UGameplayStatics::OpenLevel(World, CurrentLevelName, false);
	}
}

void UMainExperimentInstance::ValidateExperiment(FValidationStruct ValidationStruct) {
	IsLoading = true;
	FString RequestBody;

	if (!FJsonObjectConverter::UStructToJsonObjectString(ValidationStruct, RequestBody)) {
		UE_LOG(LogTemp, Error, TEXT("Erro ao converter ValidationStruct para JSON"));
		return;
	}

	// 🔍 Loga o JSON antes de enviar
	UE_LOG(LogTemp, Warning, TEXT("RequestBody JSON:\n%s"), *RequestBody);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(TEXT("http://127.0.0.1:8000/validar"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			if (bWasSuccessful && Response.IsValid()) {
				FString ResponseContent = Response->GetContentAsString();
				this->HandleValidation(ResponseContent, true);
			} else {
				UE_LOG(LogTemp, Error, TEXT("Erro na requisição HTTP"));
				FString ResponseContent = "Unknow error.";
				if (Response.IsValid()) {
					ResponseContent = Response->GetResponseCode() + TEXT(" - ") + Response->GetContentAsString();
				}
				this->HandleValidation(ResponseContent, false);
			}
		}
	);
	Request->ProcessRequest();
}

// Seu método que será chamado após o sucesso da requisição
void UMainExperimentInstance::HandleValidation(const FString& ResponseContent, bool ManageToCallValidation) {
	if (ManageToCallValidation) {
		CurrentExperiment.ResultFinal.FinalResult = EnumResultFinal::ValidationResult;
		CurrentExperiment.ResultFinal.Description = "Validation Result: " + ResponseContent;
	} else {
		CurrentExperiment.ResultFinal.FinalResult = EnumResultFinal::ValidationCallFailed;
		CurrentExperiment.ResultFinal.Description = "Failed to Call: " + ResponseContent;
	}
	
	Experiments.Add(CurrentExperiment);
	IsLoading = false;
	ResetLevel();
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

FExperimentResult UMainExperimentInstance::GetExperimentById(int32 Id) const {
	for (const FExperimentResult& Experiment : Experiments) {
		if (Experiment.ExperimentId == Id) {
			return Experiment;
		}
	}
	return FExperimentResult(); // Retorna um objeto vazio se não encontrar
}

void UMainExperimentInstance::IncrementSeconds() {
	CurrentExperiment.WallClockInSeconds += 0.01f;
}

FExperimentResult& UMainExperimentInstance::GetCurrentExperiment() {
	return CurrentExperiment;
}

FExperimentResult UMainExperimentInstance::GetLastExperimentResult() {
	if (Experiments.IsEmpty()) {
		return FExperimentResult();
	}

	return Experiments.Last();
}
