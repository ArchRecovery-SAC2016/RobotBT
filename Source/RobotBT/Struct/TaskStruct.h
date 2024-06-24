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
struct FTaskPrecondition {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Predicate;

    UPROPERTY(BlueprintReadOnly)
    FString Vars;

    UPROPERTY(BlueprintReadOnly)
    FString VarTypes;
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
    TArray<FTaskPrecondition> Preconditions;

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