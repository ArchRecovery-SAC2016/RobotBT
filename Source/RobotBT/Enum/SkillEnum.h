/** This is a list of all types that exits in the game*/
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESkillEnum: uint8 {
	NONE			    UMETA(DisplayName = "NONE"),
	MOVE_FURNITURE	    UMETA(DisplayName = "MOVE_FURNITURE"),
	OPEN_DOOR		    UMETA(DisplayName = "OPEN_DOOR"),
	CLEAN_ROOM 		    UMETA(DisplayName = "CLEAN_ROOM"),
	SANITIZE_ROBOT	    UMETA(DisplayName = "SANITIZE_ROBOT"),
};

class ESkillEnumHelper {
public:
    static FString GetDisplayName(ESkillEnum Reason) {
        const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESkillEnum"), true);
        if (!EnumPtr) {
            return FString("Invalid Enum");
        }

        return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Reason)).ToString();
    }
};