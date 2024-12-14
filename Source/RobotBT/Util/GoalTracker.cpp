#include "GoalTracker.h"


bool UGoalTracker::EvaluateCreationCondition(FGoalModel GoalModel, const FString& GoalId, const TMap<FString, FString>& WorldState) {
    FGoalNode* Goal = GoalModel.FindNodeById(GoalId);
    if (Goal) {
        Goal->IsCreated = EvaluateCondition(Goal->CreationCondition, WorldState);
        return Goal->IsCreated;
    }
    return false;
}

bool UGoalTracker::EvaluateAchieveCondition(FGoalModel GoalModel, const FString& GoalId, const TMap<FString, FString>& WorldState) {
    FGoalNode* Goal = GoalModel.FindNodeById(GoalId);
    if (Goal && Goal->IsCreated) {
        Goal->IsAchieved = EvaluateCondition(Goal->AchieveCondition, WorldState);
        return Goal->IsAchieved;
    }
    return false;
}

FGoalModel UGoalTracker::Evaluate_G2(FGoalModel GoalModel, TArray<ARoomPreparation*> WorldRooms) {
    FGoalNode* CurrentGoal = GoalModel.FindNodeById("f942813f-cc19-456b-bed4-7e56c17f7bd4");
    if (CurrentGoal == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("No Goal found for FetchRoomsToBePrepared!"));
    }

	for (auto Room : WorldRooms) {
		if (Room->IsRoomPrepared()) {
            CurrentGoal->IsAchieved = true;
			return GoalModel;
		}
	}
    CurrentGoal->IsAchieved = false;
	return GoalModel;
}

FGoalModel UGoalTracker::Evaluate_G3(FGoalModel GoalModel, TArray <ARoomPreparation*> WorldRooms) {
    FGoalNode* CurrentGoal = GoalModel.FindNodeById("4ba74b3c-1ef2-4ac3-8b0b-2bc35cbdef98");
    if (CurrentGoal == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("No Goal found for Rooms Are Prepared!"));
    }

    bool AllRoomsPrepared = true;
    for (auto Room : WorldRooms) {
        if (Room->IsRoomPrepared()) {
            AllRoomsPrepared = false;
        }
    }

    CurrentGoal->IsAchieved = AllRoomsPrepared;
    
    return GoalModel;
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
