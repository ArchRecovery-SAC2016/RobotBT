/** This is a list of all types that exits in the game*/
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EActionsEnum: uint8 {
	NONE			UMETA(DisplayName = "NONE"),
	WAITING			UMETA(DisplayName = "WAITING"),
	CLEAN_ROOM		UMETA(DisplayName = "CLEAN_ROOM"),
	NOVE_FORNITURE	UMETA(DisplayName = "NOVE_FORNITURE"),
	SANITIZE_ROBOT 	UMETA(DisplayName = "SANITIZE_ROBOT"),
	OPEN_DOOR 		UMETA(DisplayName = "OPEN_DOOR"),
    
};
