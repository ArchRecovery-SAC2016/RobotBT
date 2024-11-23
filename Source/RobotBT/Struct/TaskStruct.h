#pragma once

#include "CoreMinimal.h"
#include "TaskStruct.generated.h"

USTRUCT(BlueprintType)
struct FTaskArgument {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Key;

    UPROPERTY(BlueprintReadOnly)
    FString Value;
};

USTRUCT(BlueprintType)
struct FTaskEffect {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Predicate;

    UPROPERTY(BlueprintReadOnly)
    FString Vars;

    UPROPERTY(BlueprintReadOnly)
    FString VarTypes;
};

USTRUCT(BlueprintType)
struct FTaskDecomposition {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    FString Arguments;
};

USTRUCT(BlueprintType)
struct FPredicate {
    GENERATED_BODY()

    FString Variable; // ex: "RoomA"
    FString Condition; // ex: "door_open"
    bool bNegated; // true se for "not"

    FPredicate() {}

    // Construtor para inicializar os valores
    FPredicate(const FString& PredicateStr) {
        // Parse da string para separar "not", variável e condição
        if (PredicateStr.StartsWith("not ")) {
            bNegated = true;
            ParsePredicate(PredicateStr.Mid(4)); // Remove o "not " e parseia o resto
        }
        else {
            bNegated = false;
            ParsePredicate(PredicateStr);
        }
    }

private:
    void ParsePredicate(const FString& PredicateStr) {
        int32 DotIndex;
        if (PredicateStr.FindChar('.', DotIndex)) {
            Variable = PredicateStr.Left(DotIndex);
            Condition = PredicateStr.Mid(DotIndex + 1);
        }
        else {
            // Caso o predicado esteja mal formado
            Variable = PredicateStr;
            Condition = "";
        }
    }
};

USTRUCT(BlueprintType)
struct FTask {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Id;

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    TArray<FTaskArgument> Arguments;

    UPROPERTY(BlueprintReadOnly)
    TArray<FTaskArgument> ArgumentsValues;

    UPROPERTY(BlueprintReadOnly)
    FString Locations;

    UPROPERTY(BlueprintReadOnly)
    FString RobotsNumFixed;

    UPROPERTY(BlueprintReadOnly)
    FString RobotsNum;

    UPROPERTY(BlueprintReadOnly)
    TArray<FPredicate> Preconditions;

    UPROPERTY(BlueprintReadOnly)
    TArray<FTaskEffect> Effects;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> TriggeringEvents;

    UPROPERTY(BlueprintReadOnly)
    TMap<FString, FTaskDecomposition> Decomposition;

    UPROPERTY(BlueprintReadOnly)
    FString Group;

    UPROPERTY(BlueprintReadOnly)
    FString Divisible;
};