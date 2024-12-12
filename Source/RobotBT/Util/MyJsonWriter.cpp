#include "MyJsonWriter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"


void UMyJsonWriter::AddToJsonFile(TArray<FExperimentResult> Results, FString ExperimentName, int32 ScenarioId) {

	FString Path = "Data/" + ExperimentName;

	if (ScenarioId != -1) {
		Path += "/Scenario_" + FString::FromInt(ScenarioId) + "/";
	}

	// Gerar o nome do arquivo com data e hora
	FDateTime Now = FDateTime::Now();
	FString Timestamp = Now.ToString(TEXT("%Y-%m-%d_%H-%M-%S"));
	FilePath = FPaths::ProjectDir() + Path + TEXT("Experiment_") + Timestamp + TEXT(".json");

    FToJson NewJson;
    NewJson.Result = Results;

    FString JSONPayload;
    FJsonObjectConverter::UStructToJsonObjectString(NewJson, JSONPayload, 0, 0);
	
	bool bSuccess = FFileHelper::SaveStringToFile(JSONPayload, *FilePath);

	if (bSuccess) {
		UE_LOG(LogTemp, Log, TEXT("Arquivo JSON criado com sucesso: %s"), *FilePath);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Falha ao criar o arquivo JSON: %s"), *FilePath);
	}
}
