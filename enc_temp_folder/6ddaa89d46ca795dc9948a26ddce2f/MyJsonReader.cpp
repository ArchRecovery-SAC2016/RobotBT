#include "MyJsonReader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

TMap<FString, FTask> UMyJsonReader::ReadTaskFromFile(FString Experiment, int32 ScenarioId) {
	TMap<FString, FTask> Tasks;

    FString Path = "Data/ " + Experiment;

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
    } else {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON"));
    }
  
	return Tasks;

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
                    // Converte diretamente para FPredicate e adiciona ao array de precondições da tarefa
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
                Task.Decomposition.Add(DecompKey, Decomp);
            }
        }

        Tasks.Add(TaskKey, Task);
    }

    return Tasks;
}

void UMyJsonReader::WriteStringToFile(FString FilePath, FString String) {
    if (!FFileHelper::SaveStringToFile(String, *FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("[UMyJsonReader::WriteStringToFile] Failed to save file"));
    } else {
        UE_LOG(LogTemp, Display, TEXT("[UMyJsonReader::ReadStringFromFile] Success on save file"));
    }

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