﻿#include "MyJsonReader.h"

#include "GoalTracker.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Struct/WorldRoomDataStruct.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

TMap<FString, FTask> UMyJsonReader::ReadTaskFromFile(FString Experiment, int32 ScenarioId) {
    TMap<FString, FTask> Tasks;

    FString Path = "Data/" + Experiment;

    if (ScenarioId != -1) {
        Path += "/Scenario_" + FString::FromInt(ScenarioId) + "/task_output.json";
    }

    FString FilePath = FPaths::ProjectContentDir() + Path;
    FString JsonString = ReadStringFromFile(FilePath);

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) {
        const TSharedPtr<FJsonObject>* TasksObject;
        if (JsonObject->TryGetObjectField(TEXT("tasks"), TasksObject)) {
            return ReadTasks(*TasksObject);
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON"));
    }

    return Tasks;
}

TArray<FWorldRoomDataStruct> UMyJsonReader::LoadWorldData(FString Experiment, int32 ScenarioId) {
    TArray<FWorldRoomDataStruct> WorldDataArray;

    // Monta o caminho do arquivo
    FString Path = "Data/" + Experiment;
    if (ScenarioId != -1) {
        Path += "/Scenario_" + FString::FromInt(ScenarioId) + "/World_db.json";
    }
    FString FilePath = FPaths::ProjectContentDir() + Path;

    // L� o conteudo do arquivo
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file at: %s"), *FilePath);
        return WorldDataArray;
    }

    // Analisa o JSON
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) {
        // Obt�m o array "rooms" do JSON
        TArray<TSharedPtr<FJsonValue>> RoomsArray = JsonObject->GetObjectField("world_db")->GetArrayField("rooms");

        // Converte cada objeto JSON para FWorldRoomDataStruct
        for (const TSharedPtr<FJsonValue>& RoomValue : RoomsArray) {
            FWorldRoomDataStruct RoomData;
            if (RoomValue->Type == EJson::Object && RoomValue->AsObject().IsValid()) {
                TSharedPtr<FJsonObject> RoomObject = RoomValue->AsObject();

                RoomData.Name = RoomObject->GetStringField("name");
                RoomData.Location = RoomObject->GetStringField("location");
                RoomData.bIsClean = RoomObject->GetBoolField("is_clean");
                RoomData.bIsPrepared = RoomObject->GetBoolField("is_prepared");
                RoomData.bDoorOpen = RoomObject->GetBoolField("door_open");

                WorldDataArray.Add(RoomData);
            }
            else {
                UE_LOG(LogTemp, Error, TEXT("RoomValue is not a valid JSON object."));
            }
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON file: %s"), *FilePath);
    }

    return WorldDataArray;
}


FString UMyJsonReader::ReadStringFromFile(FString FilePath) {
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("[UMyJsonReader::ReadStringFromFile] File not found"));
        return "";
    }

    FString RetString = "";

    if (!FFileHelper::LoadFileToString(RetString, *FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("[UMyJsonReader::ReadStringFromFile] Failed to load file"));
        return "";
    }

    UE_LOG(LogTemp, Display, TEXT("[UMyJsonReader::ReadStringFromFile] Success on read file"));
    return RetString;
}


TMap<FString, FTask> UMyJsonReader::ReadTasks(const TSharedPtr<FJsonObject>& TasksObject) {
    TMap<FString, FTask> Tasks;

    for (const auto& TaskItem : TasksObject->Values) {
        FString TaskKey = TaskItem.Key;
        TSharedPtr<FJsonObject> TaskObject = TaskItem.Value->AsObject();

        FTask Task;

        TaskObject->TryGetStringField(TEXT("id"), Task.Id);
        TaskObject->TryGetStringField(TEXT("name"), Task.Name);
        TaskObject->TryGetStringField(TEXT("locations"), Task.Locations);
        TaskObject->TryGetStringField(TEXT("group"), Task.Group);
        TaskObject->TryGetStringField(TEXT("divisible"), Task.Divisible);

        const TSharedPtr<FJsonObject>* ArgumentsObject;
        if (TaskObject->TryGetObjectField(TEXT("arguments"), ArgumentsObject)) {
            for (const auto& ArgItem : ArgumentsObject->Get()->Values) {
                FTaskArgument Arg;
                Arg.Key = ArgItem.Key;
                Arg.Value = ArgItem.Value->AsString();
                Task.Arguments.Add(Arg);
            }
        }

        const TSharedPtr<FJsonObject>* ArgumentsValuesObject;
        if (TaskObject->TryGetObjectField(TEXT("arguments_values"), ArgumentsValuesObject)) {
            for (const auto& ArgItem : ArgumentsValuesObject->Get()->Values) {
                FTaskArgument Arg;
                Arg.Key = ArgItem.Key;
                Arg.Value = ArgItem.Value->AsString();
                Task.ArgumentsValues.Add(Arg);
            }
        }

        const TSharedPtr<FJsonObject>* RobotsNumObject;
        if (TaskObject->TryGetObjectField(TEXT("robots_num"), RobotsNumObject)) {
            RobotsNumObject->Get()->TryGetStringField(TEXT("fixed"), Task.RobotsNumFixed);
            RobotsNumObject->Get()->TryGetStringField(TEXT("num"), Task.RobotsNum);
        }

        const TArray<TSharedPtr<FJsonValue>>* PreconditionsArray;
        if (TaskObject->TryGetArrayField(TEXT("preconditions"), PreconditionsArray)) {
            for (const auto& Precondition : *PreconditionsArray) {
                TSharedPtr<FJsonObject> PreconditionObject = Precondition->AsObject();
                FString PredicateStr;
                if (PreconditionObject->TryGetStringField(TEXT("predicate"), PredicateStr)) {
                    // Converte diretamente para FPredicate e adiciona ao array de precondi��es da tarefa
                    FPredicate Predicate(PredicateStr);
                    Task.Preconditions.Add(Predicate);
                }
            }
        }

        const TArray<TSharedPtr<FJsonValue>>* EffectsArray;
        if (TaskObject->TryGetArrayField(TEXT("effects"), EffectsArray)) {
            for (const auto& Effect : *EffectsArray) {
                TSharedPtr<FJsonObject> EffectObject = Effect->AsObject();
                FTaskEffect TaskEffect;
                EffectObject->TryGetStringField(TEXT("predicate"), TaskEffect.Predicate);
                EffectObject->TryGetStringField(TEXT("vars"), TaskEffect.Vars);
                EffectObject->TryGetStringField(TEXT("var_types"), TaskEffect.VarTypes);
                Task.Effects.Add(TaskEffect);
            }
        }

        const TArray<TSharedPtr<FJsonValue>>* TriggeringEventsArray;
        if (TaskObject->TryGetArrayField(TEXT("triggering_events"), TriggeringEventsArray)) {
            for (const auto& Event : *TriggeringEventsArray) {
                Task.TriggeringEvents.Add(Event->AsString());
            }
        }

        const TSharedPtr<FJsonObject>* DecompositionObject;
        if (TaskObject->TryGetObjectField(TEXT("decomposition"), DecompositionObject)) {
            for (const auto& DecompItem : DecompositionObject->Get()->Values) {
                FString DecompKey = DecompItem.Key;
                TSharedPtr<FJsonObject> DecompObject = DecompItem.Value->AsObject();
                FTaskDecomposition Decomp;
                DecompObject->TryGetStringField(TEXT("name"), Decomp.Name);
                DecompObject->TryGetStringField(TEXT("arguments"), Decomp.Arguments);
                Task.Decomposition.Add(Decomp);
            }
        }

        Tasks.Add(TaskKey, Task);
    }

    return Tasks;
}

void UMyJsonReader::WriteStringToFile(FString FilePath, FString String) {
    if (!FFileHelper::SaveStringToFile(String, *FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("[UMyJsonReader::WriteStringToFile] Failed to save file"));
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[UMyJsonReader::ReadStringFromFile] Success on save file"));
    }
}
FGoalModel UMyJsonReader::ReadGoalModel(FString Experiment, int32 ScenarioId) {
    FString Path = "Data/" + Experiment;
    if (ScenarioId != -1) {
        Path += "/Scenario_" + FString::FromInt(ScenarioId) + "/goal_model.json";
    }
    FString FilePath = FPaths::ProjectContentDir() + Path;

    // Read file
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Goal Model from file at: %s"), *FilePath);
        return FGoalModel();
    }

    FGoalModel GoalModel;

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) {
        const TArray<TSharedPtr<FJsonValue>>* ActorsArray;
        if (JsonObject->TryGetArrayField("actors", ActorsArray)) {
            for (const auto& ActorValue : *ActorsArray) {
                const TSharedPtr<FJsonObject>* ActorObject;
                if (ActorValue->TryGetObject(ActorObject)) {
                    // Para cada ator, vamos pegar os nós (nodes)
                    const TArray<TSharedPtr<FJsonValue>>* NodesArray;
                    if ((*ActorObject)->TryGetArrayField("nodes", NodesArray)) {
                        for (const auto& NodeValue : *NodesArray) {
                            const TSharedPtr<FJsonObject>* NodeObject;
                            if (NodeValue->TryGetObject(NodeObject)) {
                                FGoalNode NewNode;
                                NewNode.Id = (*NodeObject)->GetStringField("id");
                                NewNode.Text = (*NodeObject)->GetStringField("text");

                                // Lê as propriedades do nó que estão em 'customProperties'
                                const TSharedPtr<FJsonObject>* CustomPropertiesObject;
                                if ((*NodeObject)->TryGetObjectField("customProperties", CustomPropertiesObject)) {
                                    // Acessa as propriedades dentro de customProperties
                                    NewNode.GoalType = (*CustomPropertiesObject)->GetStringField("GoalType");
                                    NewNode.AchieveCondition = (*CustomPropertiesObject)->GetStringField("AchieveCondition");
                                    NewNode.CreationCondition = (*CustomPropertiesObject)->GetStringField("CreationCondition");
                                    NewNode.QueriedProperty = (*CustomPropertiesObject)->GetStringField("QueriedProperty");
                                    NewNode.Location = (*CustomPropertiesObject)->GetStringField("Location");

                                    // Controles
                                    FString ControlsValue;
                                    if ((*CustomPropertiesObject)->TryGetStringField("Controls", ControlsValue)) {
                                        NewNode.Controls.Add("Controls", *ControlsValue); // Aqui armazenamos o valor como string
                                    }

                                    // Monitores
                                    FString MonitorsValue;
                                    if ((*CustomPropertiesObject)->TryGetStringField("Monitors", MonitorsValue)) {
                                        NewNode.Monitors.Add(*MonitorsValue);
                                    }
                                }

                                // Adiciona o nó no GoalModel
                                GoalModel.Nodes.Add(NewNode);
                            }
                        }
                    }
                }
            }
        }
    }

    return GoalModel;
}



void UMyJsonReader::ShowLogMessage(const FString& Message, EMessageColorEnum Type) {
    FColor Color = FColor::Emerald;

    if (Type == EMessageColorEnum::ERROR) {
        Color = FColor::Red;
    }
    else if (Type == EMessageColorEnum::WARNING) {
        Color = FColor::Yellow;
    }
    else if (Type == EMessageColorEnum::INFO) {
        Color = FColor::Green;
    }
    else if (Type == EMessageColorEnum::SUCCESS) {
        Color = FColor::Blue;
    }

    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, Message);
    }

    UE_LOG(LogTemp, Display, TEXT("[UMyJsonReader::ShowLogMessage] %s"), *Message);
}

