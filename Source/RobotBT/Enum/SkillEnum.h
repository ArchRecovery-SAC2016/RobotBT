/** This is a list of all types that exits in the game*/
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESkillEnum: uint8 {
	NONE			UMETA(DisplayName = "none"),
	MOVE_FURNITURE	UMETA(DisplayName = "move-furniture"),
	OPEN_DOOR		UMETA(DisplayName = "open-door"),
	CLEAN_ROOM 		UMETA(DisplayName = "clean-room"),
	SANITIZE_ROBOT	UMETA(DisplayName = "sanitize-robot"),
};

class ESkillEnumHelper {
public:
    static FString GetDisplayName(ESkillEnum Reason) {
        const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFailureReasonEnum"), true);
        if (!EnumPtr) {
            return FString("Invalid Enum");
        }

        return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Reason)).ToString();
    }
};