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
    FString Timestamp = Now.ToString(TEXT("yyyy-MM-dd_HH-mm-ss"));
    FilePath = FPaths::ProjectDir() + Path + TEXT("Experiment_") + Timestamp + TEXT(".csv");

    // Definir os cabeçalhos do CSV
    FString CSVContent;
    CSVContent.Append(TEXT("ExperimentId,Approach,ExperimentTime,"));
    CSVContent.Append(TEXT("TaskName,Location,SuccessResult,FailureReasonEnum,"));
    CSVContent.Append(TEXT("InitialRobotName,InitialRobotSquad,InitialBatteryCharge,InitialSkills,InitialColor,InitialSpeed,InitialLocationX,InitialLocationY,InitialLocationZ,"));
    CSVContent.Append(TEXT("EndRobotName,EndRobotSquad,EndBatteryCharge,EndSkills,EndColor,EndSpeed,EndLocationX,EndLocationY,EndLocationZ\n"));

    // Criar o arquivo
    bool bSuccess = FFileHelper::SaveStringToFile(CSVContent, *FilePath);

    if (bSuccess) {
        UE_LOG(LogTemp, Log, TEXT("Arquivo CSV criado com sucesso: %s"), *FilePath);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Falha ao criar o arquivo CSV: %s"), *FilePath);
    }
}


void UMyCSVReader::AddToFile(FExperimentResult Result) {
    // Verifica se o arquivo existe
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("O arquivo CSV não existe. Crie-o antes de escrever."));
        return;
    }

    FString Line;

    // Itera sobre cada tarefa no experimento
    for (const FTaskResult& Task : Result.TaskResults) {

        const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESkillEnum"), true);
        if (!EnumPtr) {
            UE_LOG(LogTemp, Error, TEXT("O arquivo CSV não existe. Crie-o antes de escrever."));
            return;
        }

        // Cria os dados para a linha
        FString InitialSkills;
        for (const FSkill& Skill : Task.InitialRobotsProperties.Skills) {
            InitialSkills += ESkillEnumHelper::GetDisplayName(Skill.SkillEnum) + ";";
        }

        FString EndSkills;
        for (const FSkill& Skill : Task.EndRobotsProperties.Skills) {
            EndSkills += ESkillEnumHelper::GetDisplayName(Skill.SkillEnum) + ";";
        }

        Line.Append(FString::Printf(TEXT("%d,%s,%.2f,"),
            Result.ExperimentId,
            *Result.Approach,
            Result.ExperimentTime));

        Line.Append(FString::Printf(TEXT("%s,%s,%s,%s,"),
            *Task.TaskName,
            *Task.Location,
            Task.SuccessResult ? TEXT("true") : TEXT("false"),
            *UEnum::GetValueAsString(Task.FailureReasonEnum)));

        Line.Append(FString::Printf(TEXT("%s,%s,%.2f,%s,%s,%.2f,%.2f,%.2f,%.2f,"),
            *Task.InitialRobotsProperties.Name,
            *Task.InitialRobotsProperties.SquadName,
            Task.InitialRobotsProperties.Battery.Charge,
            *InitialSkills,
            *Task.InitialRobotsProperties.Color,
            Task.InitialRobotsProperties.Speed,
            Task.InitialRobotsProperties.InitialLocation.X,
            Task.InitialRobotsProperties.InitialLocation.Y,
            Task.InitialRobotsProperties.InitialLocation.Z));

        Line.Append(FString::Printf(TEXT("%s,%s,%.2f,%s,%s,%.2f,%.2f,%.2f,%.2f\n"),
            *Task.EndRobotsProperties.Name,
            *Task.EndRobotsProperties.SquadName,
            Task.EndRobotsProperties.Battery.Charge,
            *EndSkills,
            *Task.EndRobotsProperties.Color,
            Task.EndRobotsProperties.Speed,
            Task.EndRobotsProperties.InitialLocation.X,
            Task.EndRobotsProperties.InitialLocation.Y,
            Task.EndRobotsProperties.InitialLocation.Z));
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
