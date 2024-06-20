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
	void NativeConstruct();

	UFUNCTION()
	void Initiate(const FRoomPreparationStruct& RoomPreparation);

	UFUNCTION()
	void ChangeWorldKnowledge();

	void ChangeProperties(bool bIsClean, bool bIsPrepared, bool bDoorOpen);

	UFUNCTION()
	void SetIsClear(bool NewValue);

	UFUNCTION()
	void SetIsPrepared(bool NewValue);

	UFUNCTION()
	void SetDoorOpen(bool NewValue);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RoomName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RoomLocation;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCheckBox* IsClear_Check;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCheckBox* IsPrepared_Check;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCheckBox* DoorOpen_Check;

	/** The button of this item. Trow an event when clicked. The EquipmentWidget receives this event and open the panel */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* ApplyButton;


};
