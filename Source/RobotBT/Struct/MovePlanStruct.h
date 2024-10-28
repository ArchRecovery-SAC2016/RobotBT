#pragma once

#include "CoreMinimal.h"
#include "MovePlanStruct.generated.h"

USTRUCT(BlueprintType)
struct FMovePlanStruct {
    GENERATED_BODY()

    UPROPERTY(EditInstanceOnly)
    FString Identifier;

    UPROPERTY(EditInstanceOnly)
    int32 StartKey;

    UPROPERTY(EditInstanceOnly)
    int32 EndKey;
};