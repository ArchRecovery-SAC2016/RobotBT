#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RobotBT/Struct/RoomPreparationStruct.h"
#include "RoomPropertiesWidget.generated.h"

/**
 * Show the properties of the rooms in the game.
 */
UCLASS()
class ROBOTBT_API URoomPropertiesWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UFUNCTION()
	void InitialLoad(FRoomPreparationStruct Room);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RoomName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RoomLocation;


};
