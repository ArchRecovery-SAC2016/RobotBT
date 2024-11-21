#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFailureReasonEnum: uint8 {
	LowBattery        UMETA(DisplayName = "Low Battery"),
    Timeout           UMETA(DisplayName = "Timeout"),
    SkillFailure      UMETA(DisplayName = "Skill Failure"),
    ObstacleDetected  UMETA(DisplayName = "Obstacle Detected"),
    UnknownReason     UMETA(DisplayName = "Unknown Reason")
};

class EFailureReasonEnumHelper {
public:
    static FString GetDisplayName(EFailureReasonEnum Reason) {
        const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFailureReasonEnum"), true);
        if (!EnumPtr) {
            return FString("Invalid Enum");
        }

        return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Reason)).ToString();
    }
};