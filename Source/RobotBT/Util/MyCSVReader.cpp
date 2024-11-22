#include "MyCSVReader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


void UMyCSVReader::CreateCSVFile(bool NewFile) {
    FString FilePath;

    if (NewFile) {
        // Gerar o nome do arquivo com data e hora
        FDateTime Now = FDateTime::Now();
        FString Timestamp = Now.ToString(TEXT("yyyy-MM-dd_HH-mm-ss"));
        FilePath = FPaths::ProjectDir() + TEXT("Experiment_") + Timestamp + TEXT(".csv");
    }
    else {
        // Nome fixo para o arquivo
        FilePath = FPaths::ProjectDir() + TEXT("Experiment.csv");
    }

    // Definir os cabeçalhos do CSV
    FString CSVContent;
    CSVContent.Append(TEXT("RobotName,Speed,BatteryCharge,FailureReason\n")); // Cabeçalhos das colunas

    // Criar o arquivo
    bool bSuccess = FFileHelper::SaveStringToFile(CSVContent, *FilePath);

    if (bSuccess) {
        UE_LOG(LogTemp, Log, TEXT("Arquivo CSV criado com sucesso: %s"), *FilePath);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Falha ao criar o arquivo CSV: %s"), *FilePath);
    }
}

void UMyCSVReader::WriteCSVFile(FRobotProperties RobotProperties, EFailureReasonEnum FailureReason) {
	// Define o caminho do arquivo
	FString FilePath = FPaths::ProjectDir() + TEXT("Experimento.csv");

	// Verifica se o arquivo existe
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath)) {
		UE_LOG(LogTemp, Error, TEXT("O arquivo CSV não existe. Crie-o antes de escrever."));
		return;
	}

	// Converte os dados em uma linha CSV
	FString FailureReasonString = UEnum::GetValueAsString(FailureReason);
	FString Line = FString::Printf(
		TEXT("%s,%.2f,%.2f,%s\n"),
		*RobotProperties.Name,
		RobotProperties.Speed,
		RobotProperties.Battery.Charge,
		*FailureReasonString
	);

	// Adiciona a nova linha ao arquivo
	bool bSuccess = FFileHelper::SaveStringToFile(Line, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

	if (bSuccess) {
		UE_LOG(LogTemp, Log, TEXT("Dados adicionados ao arquivo CSV: %s"), *Line);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Falha ao adicionar dados ao arquivo CSV."));
	}
}