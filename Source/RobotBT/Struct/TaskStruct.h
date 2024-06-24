#pragma once

#include "CoreMinimal.h"
#include "TaskStruct.generated.h"

USTRUCT(BlueprintType)
struct FTaskStruct {
    GENERATED_BODY()

public:
    FString id;

    FString name;

    FString arguments;

    FString arguments_values;

    FString locations;

    FString robots_num;

    FString preconditions;

    FString effects;

    FString triggering_events;

    FTaskDecompositionStruct decomposition;

    bool group;

    bool divisible;

    // Constructor

    FTaskStruct() {}

};
