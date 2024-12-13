#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GoalTracker.generated.h"


USTRUCT(BlueprintType)
struct FGoalMonitor {
    GENERATED_BODY()



};

USTRUCT(BlueprintType)
struct FGoalNode {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    FString Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    FString Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    FString GoalType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    TMap<FString, FString> Controls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    TArray<FString> Monitors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    FString QueriedProperty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    FString Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    FString AchieveCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    FString CreationCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    bool bIsAchieved = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    bool bIsCreated = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    float Progress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    float ElapsedTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Node")
    bool Result;

    FGoalNode() {}

    FGoalNode(const FString& InId, const FString& InText)
        : Id(InId), Text(InText) {
    }
};

// Estrutura representando o modelo de goals
USTRUCT(BlueprintType)
struct FGoalModel {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Model")
    TArray<FGoalNode> Nodes;

    FGoalNode* FindNodeById(const FString& Id) {
        return Nodes.FindByPredicate([&](const FGoalNode& Node) { return Node.Id == Id; });
    }
};

// Classe principal de rastreamento de goals
UCLASS()
class ROBOTBT_API UGoalTracker : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Evaluator")
    FGoalModel GoalModel;

    // Avaliar a condição de criação de um goal
    UFUNCTION(BlueprintCallable, Category = "Goal Evaluator")
    bool EvaluateCreationCondition(const FString& GoalId, const TMap<FString, FString>& WorldState);

    // Avaliar a condição de realização de um goal
    UFUNCTION(BlueprintCallable, Category = "Goal Evaluator")
    bool EvaluateAchieveCondition(const FString& GoalId, const TMap<FString, FString>& WorldState);

private:
    // Função auxiliar para avaliar uma condição genérica
    bool EvaluateCondition(const FString& Condition, const TMap<FString, FString>& WorldState);
};