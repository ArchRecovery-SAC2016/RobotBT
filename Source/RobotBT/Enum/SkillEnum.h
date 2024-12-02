/** This is a list of all types that exits in the game*/
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESkillEnum: uint8 {
	NONE			    UMETA(DisplayName = "none"),
	MOVE_FURNITURE	    UMETA(DisplayName = "MoveFurniture"),
	OPEN_DOOR		    UMETA(DisplayName = "OpenDoor"),
	CLEAN_ROOM 		    UMETA(DisplayName = "CleanRoom"),
	SANITIZE_ROBOT	    UMETA(DisplayName = "SanitizeRobot"),
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