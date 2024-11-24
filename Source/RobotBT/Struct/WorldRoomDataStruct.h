#pragma once

#include "CoreMinimal.h"
#include "WorldRoomDataStruct.generated.h"

USTRUCT(BlueprintType)
struct FWorldRoomDataStruct {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString Name;

    UPROPERTY(BlueprintReadWrite)
    FString Location;

    UPROPERTY(BlueprintReadWrite)
    bool bIsClean;

    UPROPERTY(BlueprintReadWrite)
    bool bIsPrepared;

    UPROPERTY(BlueprintReadWrite)
    bool bDoorOpen;
};