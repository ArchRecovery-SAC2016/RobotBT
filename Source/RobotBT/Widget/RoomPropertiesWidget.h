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

	UFUNCTION(BlueprintCallable)
	void SetRoomName(FString NewValue);

	UFUNCTION(BlueprintCallable)
	void SetLocation(FString NewValue);

	UFUNCTION()
	void SetIsClear(bool NewValue);

	UFUNCTION()
	void SetIsPrepared(bool NewValue);

	UFUNCTION()
	void SetDoorOpen(bool NewValue);

	void ChangeProperties(bool bIsClean, bool bIsPrepared, bool bDoorOpen);

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

};
