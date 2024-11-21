/** This is a list of all types that exits in the game*/
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
