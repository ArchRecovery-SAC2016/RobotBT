#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFailureReasonEnum: uint8 {
    None              UMETA(DisplayName = "None"),
	LowBattery        UMETA(DisplayName = "Low Battery"),
    Timeout           UMETA(DisplayName = "Timeout"),
    SkillNotFound     UMETA(DisplayName = "Skill Not Found"),
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