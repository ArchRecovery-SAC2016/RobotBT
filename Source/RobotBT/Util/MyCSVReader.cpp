#include "MyCSVReader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UMyCSVReader::CreateCSVFile(FString ExperimentName, int32 ScenarioId) {
    FString Path = "Data/" + ExperimentName;

    if (ScenarioId != -1) {
        Path += "/Scenario_" + FString::FromInt(ScenarioId) + "/";
    }

    // Gerar o nome do arquivo com data e hora
    FDateTime Now = FDateTime::Now();
    FString Timestamp = Now.ToString(TEXT("%Y-%m-%d_%H-%M-%S"));
    FilePath = FPaths::ProjectDir() + Path + TEXT("Experiment_") + Timestamp + TEXT(".csv");

    // Definir os cabe�alhos do CSV
    FString CSVContent;
    CSVContent.Append(TEXT("ExperimentId,Approach,TaskFinishTime,"));
    CSVContent.Append(TEXT("SkillName,SkillChanceToFail,SkillBatteryConsume, Location,SuccessResult,FailureReasonEnum,"));
    CSVContent.Append(TEXT("RobotName, RobotColor, BatteryCharge,BatteryDischargeRate,Skills,Speed,EndBatteryCharge,"));
    CSVContent.Append(TEXT("TimeSpent, BatterySpent\n"));
    

    // Criar o arquivo
    bool bSuccess = FFileHelper::SaveStringToFile(CSVContent, *FilePath);

    if (bSuccess) {
        UE_LOG(LogTemp, Log, TEXT("Arquivo CSV criado com sucesso: %s"), *FilePath);
    } else {
        UE_LOG(LogTemp, Error, TEXT("Falha ao criar o arquivo CSV: %s"), *FilePath);
    }
}

void UMyCSVReader::AddToFile(FExperimentResult Result) {
    // Verifica se o arquivo existe
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("O arquivo CSV nao existe. Crie-o antes de escrever."));
        return;
    }

    FString Line;

    // Itera sobre cada tarefa no experimento
    for (const FTaskResult& Task : Result.TaskResults) {

        const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESkillEnum"), true);
        if (!EnumPtr) {
            UE_LOG(LogTemp, Error, TEXT("O arquivo CSV nao existe. Crie-o antes de escrever."));
            return;
        }

        // Cria os dados para a linha
        FString InitialSkills;
        for (const FSkill& Skill : Task.InitialRobotsProperties.Skills) {
            InitialSkills += ESkillEnumHelper::GetDisplayName(Skill.SkillEnum) + "*";
        }

        Line.Append(FString::Printf(TEXT("%d,%s,%.2f,"),
            Result.ExperimentId,
            *Result.Approach,
            Result.WallClockInSeconds));

        FString FailureReason = EFailureReasonEnumHelper::GetDisplayName(Task.FailureReasonEnum);
        Line.Append(FString::Printf(TEXT("%s,%.2f,%.2f,%s,%s,%s,"),
            *Task.SkillUsed,
            Task.SkillChanceToFail,
            Task.SkillBatteryConsumeDischargeRate,
            *Task.Location,
            Task.SuccessResult ? TEXT("true") : TEXT("false"),
            *FailureReason
            ));

        Line.Append(FString::Printf(TEXT("%s,%s,%.2f,%.2f, %s,%.2f,%.2f"),
            *Task.InitialRobotsProperties.Name,
            *Task.InitialRobotsProperties.Color,
            Task.InitialRobotsProperties.Battery.Charge,
            Task.InitialRobotsProperties.Battery.DischargeRate,
            *InitialSkills,
            Task.InitialRobotsProperties.Speed,
            Task.EndRobotsProperties.Battery.Charge));

        Line.Append(FString::Printf(TEXT("%.2f,%.2f\n"),
            Task.TimeSpentOnTask,
            Task.BatterySpentOnTask));

    }

    // Adiciona a nova linha ao arquivo
    bool bSuccess = FFileHelper::SaveStringToFile(Line, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);

    if (bSuccess) {
        UE_LOG(LogTemp, Log, TEXT("Dados adicionados ao arquivo CSV: %s"), *Line);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Falha ao adicionar dados ao arquivo CSV."));
    }
}
