#include "GoalTracker.h"


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