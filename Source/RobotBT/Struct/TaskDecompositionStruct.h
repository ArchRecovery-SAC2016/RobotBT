#pragma once

#include "CoreMinimal.h"
#include "TaskDecompositionStruct.generated.h"

USTRUCT(BlueprintType)
struct FTaskDecompositionStruct {
    GENERATED_BODY()

public:
    EActionsEnum ActionsEnum;
    FString Arguments;

    // Constructor
    FTaskDecompositionStruct() {}


};
