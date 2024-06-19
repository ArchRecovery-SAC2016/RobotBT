#pragma once

#include "CoreMinimal.h"
#include "RoomPreparationStruct.generated.h"

USTRUCT(BlueprintType)
struct FRoomPreparationStruct {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
    FString Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
    bool bIsClean;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
    bool bIsPrepared;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
    bool bDoorOpen;

    // Constructor

    FRoomPreparationStruct() {}

    FRoomPreparationStruct(
        FString _Name,
        FString _Location,
        bool _bIsClean,
        bool _bIsPrepared,
        bool _bDoorOpen)
        : Name(_Name),
        Location(_Location),
        bIsClean(_bIsClean),
        bIsPrepared(_bIsPrepared),
        bDoorOpen(_bDoorOpen) {
    }
};
