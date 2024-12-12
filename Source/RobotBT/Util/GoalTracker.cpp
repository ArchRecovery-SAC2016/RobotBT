#include "GoalTracker.h"

void UGoalTracker::LoadGoalModelFromJson(const FString& JsonString) {
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) {
        const TArray<TSharedPtr<FJsonValue>>* NodesArray;
        if (JsonObject->TryGetArrayField("Nodes", NodesArray)) {
            for (const auto& NodeValue : *NodesArray) {
                const TSharedPtr<FJsonObject>* NodeObject;
                if (NodeValue->TryGetObject(NodeObject)) {
                    FGoalNode NewNode;
                    NewNode.Id = (*NodeObject)->GetStringField("Id");
                    NewNode.Text = (*NodeObject)->GetStringField("Text");
                    NewNode.GoalType = (*NodeObject)->GetStringField("GoalType");
                    NewNode.AchieveCondition = (*NodeObject)->GetStringField("AchieveCondition");
                    NewNode.CreationCondition = (*NodeObject)->GetStringField("CreationCondition");

                    // Controles
                    const TSharedPtr<FJsonObject>* ControlsObject;
                    if ((*NodeObject)->TryGetObjectField("Controls", ControlsObject)) {
                        for (const auto& Control : (*ControlsObject)->Values) {
                            NewNode.Controls.Add(Control.Key, Control.Value->AsString());
                        }
                    }

                    // Monitores
                    const TArray<TSharedPtr<FJsonValue>>* MonitorsArray;
                    if ((*NodeObject)->TryGetArrayField("Monitors", MonitorsArray)) {
                        for (const auto& MonitorValue : *MonitorsArray) {
                            NewNode.Monitors.Add(MonitorValue->AsString());
                        }
                    }

                    GoalModel.Nodes.Add(NewNode);
                }
            }
        }
    }
}

bool UGoalTracker::EvaluateCreationCondition(const FString& GoalId, const TMap<FString, FString>& WorldState) {
    FGoalNode* Goal = GoalModel.FindNodeById(GoalId);
    if (Goal) {
        Goal->bIsCreated = EvaluateCondition(Goal->CreationCondition, WorldState);
        return Goal->bIsCreated;
    }
    return false;
}

bool UGoalTracker::EvaluateAchieveCondition(const FString& GoalId, const TMap<FString, FString>& WorldState) {
    FGoalNode* Goal = GoalModel.FindNodeById(GoalId);
    if (Goal && Goal->bIsCreated) {
        Goal->bIsAchieved = EvaluateCondition(Goal->AchieveCondition, WorldState);
        return Goal->bIsAchieved;
    }
    return false;
}

bool UGoalTracker::EvaluateCondition(const FString& Condition, const TMap<FString, FString>& WorldState) {
    // Exemplo de uma simples avaliação de condição
    for (const auto& KeyValue : WorldState) {
        if (Condition.Contains(KeyValue.Key) && Condition.Contains(KeyValue.Value)) {
            return true;
        }
    }
    return false;
}